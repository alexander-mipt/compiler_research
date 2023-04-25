#define CATCH_CONFIG_MAIN
#include "bbGraph.hpp"
#include "catch.hpp"
// #include "LoopTreeBuilder.hpp"
#include "constFoldProp.hpp"
#include "memManager.hpp"

TEST_CASE("Test peepholes", "[constfold]") {
    IR::BbGraph g{};
    IR::BasicBlockManager bbs{};
    IR::InstrManager instrs{};
    auto *bb0 = bbs.create();
    auto *bb1 = bbs.create();
    auto *c1 = instrs.create(IR::OpcodeType::ICONST, IR::GroupType::CONST,
                             IR::ValueHolder(1, IR::NO_VALUEHOLDER));
    auto *c2 = instrs.create(IR::OpcodeType::ICONST, IR::GroupType::CONST,
                             IR::ValueHolder(3, IR::NO_VALUEHOLDER));
    auto *c3 = instrs.create(IR::OpcodeType::ICONST, IR::GroupType::CONST,
                             IR::ValueHolder(3, IR::NO_VALUEHOLDER));
    auto *a_add_c1_c2 =
        instrs.create(IR::OpcodeType::ADD, IR::GroupType::GENERAL64, IR::ValueHolder(0, 1));
    auto *b_add_a_c3 =
        instrs.create(IR::OpcodeType::ADD, IR::GroupType::GENERAL64, IR::ValueHolder(0, 1));

    a_add_c1_c2->push_inputs({c1, c2});
    b_add_a_c3->push_inputs({c3, a_add_c1_c2});

    bb0->push_instrs({c1, c2, c3});
    bb1->push_instrs({a_add_c1_c2, b_add_a_c3});

    bb0->throwIfNotConsistent_();
    bb1->throwIfNotConsistent_();
    g.setHeader(bb0->get_id());

    REQUIRE(g.add_node(*bb0) != G::KEY_UNDEF);
    REQUIRE(g.add_node(*bb1) != G::KEY_UNDEF);
    std::cerr << g.dump();

    doConstFoldProp(g);

    std::cerr << g.dump();
}

TEST_CASE("Test peepholes 2", "[constfold2]") {
    IR::BbGraph g{};
    IR::BasicBlockManager bbs{};
    IR::InstrManager instrs{};
    auto *bb0 = bbs.create();
    auto *bb1 = bbs.create();
    
    auto *cx = instrs.createCONST(IR::ValueHolder(0, IR::NO_VALUEHOLDER));
    auto *c0 = instrs.createCONST(IR::ValueHolder(1, IR::NO_VALUEHOLDER));
    auto *c1 = instrs.createCONST(IR::ValueHolder(2, IR::NO_VALUEHOLDER));

    auto i0 = instrs.createADD({});
    i0->push_inputs({cx, cx});
    auto i1 = instrs.createADD({});
    i1->push_inputs({i0, c0});
    auto i2 = instrs.createADD({});
    i2->push_inputs({i1, c1});

    bb0->push_instrs({cx, c0, c1});
    bb1->push_instrs({i0, i1, i2});

    bb0->throwIfNotConsistent_();
    bb1->throwIfNotConsistent_();
    g.setHeader(bb0->get_id());

    REQUIRE(g.add_node(*bb0) != G::KEY_UNDEF);
    REQUIRE(g.add_node(*bb1) != G::KEY_UNDEF);
    std::cerr << g.dump();

    doPeepholes(g, instrs);

    std::cerr << g.dump();
}