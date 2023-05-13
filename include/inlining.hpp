#pragma once
#include "bbGraph.hpp"
#include "instruction.hpp"
#include "memManager.hpp"
#include <set>

// for simplicity we assume that each call can be inlined once
// because deep copy of inline graph is not supported yet, sorry for this :(
// then we can move data from remote graph to target
// we can emulate the same calls by binding them to the same graph factory function (which produces
// some fixed g)
void inlineCFGDFG(IR::BbGraph &target, IR::BbGraph &remote, key_t start, key_t end,
                  IR::InstrBase::InputsT &inputs, IR::InstrBase* call) {

    // move constants first
    auto &remoteHeadBb = remote.accessHeader()->data();
    std::vector<IR::Instr *> instrs{};
    for (auto *instr : remoteHeadBb.access_instrs()) {
        if (instr->type() == IR::GroupType::CONST) {
            instrs.push_back(instr);
        }
    }
    for (auto *instr : instrs) {
        auto cut = remoteHeadBb.cut_instr(instr->get_id());
        target.accessHeader()->data().push_instrs({cut.second});
    }

    // bind params with inputs
    // count of params and inputs has already been checked
    std::vector<std::pair<IR::InstrBase *, IR::InstrBase *>> bindings{};
    auto inputIt = inputs.begin();
    for (auto *param : remoteHeadBb.access_instrs()) {
        if (param->type() == IR::GroupType::PARAM) {
            bindings.push_back({*inputIt, param});
            ++inputIt;
        }
    }
    // replace params on inputs in all param users
    for (auto binding : bindings) {
        auto *newInput = binding.first;
        auto *oldInput = binding.second;
        for (auto *user: oldInput->users()) {
            std::replace(user->inputs_begin(), user->inputs_end(), oldInput, newInput);
            std::replace(newInput->users_begin(), newInput->users_end(), call, user);
        }
    }

    auto rpo = remote.RPO(remote.accessHeader()->get_key());
    auto remoteStart = rpo[] for (auto it = remote.nodes_begin(); it != remote.nodes_end(); ++it) {
        if (it->first != remoteHead) {
            // copy all block ptrs except head ptr
            auto old_key = it->first;
            auto res = target.add_node(it->second->data());
            ASSERT_DEV(res == old_key, "id has been changed");
        } else {
            // move constants
        }
    }
    
}

bool checkParamsAndCallInputsEq(IR::Call *call) {
    auto paramCounter{0};
    const auto &inlineGraphHead = call->m_g->accessHeader()->data();
    for (auto it = inlineGraphHead.instr_cbegin(); it != inlineGraphHead.instr_cend(); ++it) {
        if ((*it)->type() == IR::GroupType::PARAM) {
            ++paramCounter;
        }
    }
    return (call->inputs().size() == paramCounter);
}

void doInlining(IR::BbGraph &g, IR::BasicBlockManager &bbManager) {
    auto rpo = g.RPO(g.accessHeader()->get_key());
    std::vector<IR::Call *> calls{};
    for (auto key : rpo) {
        auto &bb = g.at(key)->data();
        for (auto it = bb.instr_begin(); it != bb.instr_end(); ++it) {
            auto *instr = *it;
            ASSERT_DEV(instr, "nullptr in bb");
            ASSERT_DEV(instr->bb(), "nullptr in instr bb");
            if (instr->m_type == IR::GroupType::CALL) {
                calls.push_back(static_cast<IR::Call *>(instr));
            }
        }
    }

    std::set<const IR::BbGraph *> inlineGraps{};
    for (auto *call : calls) {

        if (!checkParamsAndCallInputsEq(call)) {
            throw std::logic_error("cannot bind params with call inputs");
        }
        if (!inlineGraps.insert(call->m_g).second) {
            throw std::logic_error("repeated call to the same graph ptr is not supported, each "
                                   "call should be referenced to unique ptr");
        }
    }

    for (auto *call : calls) {
        // skip inline for trivial calls
        // if (call->m_g->get_node_count() <= 2) {
        //     continue;
        // }
        auto *bbWithCall = call->bb();
        key_t inlineStartId = bbWithCall->get_id(); // where start of inline
        key_t inlineEndId = G::KEY_UNDEF;           // where end of inline
        std::vector<key_t> bbCallSuccessors{};

        // determine successors of bb with call instr;
        // inline blocks should be targeted to them
        auto *bbWithCallNode = g.at(bbWithCall->get_id());
        ASSERT_DEV(bbWithCallNode != nullptr, "invalid bb with call instr");
        for (auto succIt = bbWithCallNode->successors_begin();
             succIt != bbWithCallNode->successors_end(); ++succIt) {
            bbCallSuccessors.push_back(succIt->first);
        }
        // destory connections between current bb  and target bbs (succsessors)
        for (auto key : bbCallSuccessors) {
            auto res = g.delete_edge(bbWithCall->get_id(), key);
            ASSERT_DEV(res != G::KEY_UNDEF, "successor does not exist");
        }

        // detect instrs for new bb (tail instrs after call)
        auto callIt = bbWithCall->find_first_instr_occurence(call->get_id());
        ASSERT_DEV(callIt != bbWithCall->instr_end(), "no call detected");
        auto *beforeCallInstr = (*callIt)->prev();
        auto *afterCallInstr = (*callIt)->next();
        auto endIt = bbWithCall->instr_end();

        // split bb with call into 2 if call is not the end of bb
        if (afterCallInstr != nullptr) {
            // create new bb, which will contains
            // instrs from current call position up to the end of bb with that call
            auto *afterCallBb = bbManager.create();
            ASSERT_DEV(afterCallBb != nullptr, "Cannot create new bb");
            auto middleId = g.add_node(*afterCallBb);
            ASSERT_DEV(middleId != G::KEY_UNDEF, "Something wrong in graph");

            // update cfg
            // (bb with call and next bb has already benn detached, edge has been deleted above)
            // connect middle with end, update end
            for (auto key : bbCallSuccessors) {
                auto res = g.add_edge(1, middleId, key);
                ASSERT_DEV(res != G::KEY_UNDEF, "middle has not been atached");
            }
            if (bbCallSuccessors.size() > 0) {
                inlineEndId = middleId;
            }

            // cut tail instrs (with saving their cfg & dfg deps)
            IR::BasicBlock::InstrContainer instrsAfterCall{};
            auto &originInstrList = bbWithCall->access_instrs();
            auto startIt = bbWithCall->find_first_instr_occurence(afterCallInstr->get_id());
            instrsAfterCall.splice(instrsAfterCall.begin(), originInstrList, startIt, endIt);
            // push them to the new bb in the same order
            for (auto *instr : instrsAfterCall) {
                instr->set_bb(nullptr);
                afterCallBb->push_instrs({instr});
            }
        } else {
            // call cannot has more than 1 destination
            // if it is the last instr - check this invariant
            ASSERT_DEV(bbCallSuccessors.size() <= 1, "1+ dsts from bb ended with call instr");
            if (bbCallSuccessors.size() == 1) {
                inlineEndId = bbCallSuccessors.front();
            }
        }

        /*
         * Now
         * inlineStartId - id for inline head (bb with call instr) - instructions will be added here
         * inlineEndId - id for inline tail - instructions will not be added here, but it is end
         * link for inline bbflow
         */
        inlineCFGDFG(g, *(call->m_g), inlineStartId, inlineEndId, call->inputs(), static_cast<IR::InstrBase*>(call));
    }
}