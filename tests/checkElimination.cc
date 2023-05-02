#define CATCH_CONFIG_MAIN
#include "bbGraph.hpp"
#include "catch.hpp"
#include "memManager.hpp"
#include "checkElimination.hpp"

/* CFG
┌───┐  3   ┌────┐
│ 4 │ ◀─── │ 1  │
└───┘      └────┘
  │          │
  │          │ 1
  │          ▼
  │        ┌────┐
  │        │ 2  │
  │        └────┘
  │          │
  │          │ 2
  │          ▼
  │        ┌────┐
  │        │ 3  │
  │        └────┘
  │          │
  │          │ 5
  │          ▼
  │   4    ┌────┐
  └──────▶ │ 5  │
           └────┘
*/

/* BBs data

// instr_id 8 - dominated check for bb_id 3 and 5;
// then instr_ids 9, 11, 12 will be excluded   

Basic block; id: 1
--- Instrs ---
m_id: 1    m_opcd: 1    m_type: 2     
m_id: 2    m_opcd: 1    m_type: 2     
Basic block; id: 2
--- Instrs ---
m_id: 3    m_opcd: 4    m_type: 1     
m_id: 8    m_opcd: 8    m_type: 4
Basic block; id: 3
--- Instrs ---
m_id: 4    m_opcd: 4    m_type: 1     
m_id: 9    m_opcd: 8    m_type: 4     
Basic block; id: 4
--- Instrs ---
m_id: 5    m_opcd: 4    m_type: 1     
m_id: 10    m_opcd: 8    m_type: 4     
Basic block; id: 5
--- Instrs ---
m_id: 6    m_opcd: 4    m_type: 1     
m_id: 11    m_opcd: 8    m_type: 4     
m_id: 7    m_opcd: 4    m_type: 1     
m_id: 12    m_opcd: 8    m_type: 4     
*/

TEST_CASE("Test checkElimination", "[constfold2]") {
    IR::BbGraph g{};
    IR::BasicBlockManager bbs{};
    IR::InstrManager instrs{};
    
    auto *bb1 = bbs.create();
    auto *bb2 = bbs.create();
    auto *bb3 = bbs.create();
    auto *bb4 = bbs.create();
    auto *bb5 = bbs.create();

    auto *c0 = instrs.createCONST(IR::ValueHolder(1, IR::NO_VALUEHOLDER));
    auto *c1 = instrs.createCONST(IR::ValueHolder(2, IR::NO_VALUEHOLDER));

    auto *i1 = instrs.createADD({});
    i1->push_inputs({c0, c0});
    auto *i2 = instrs.createADD({});
    i2->push_inputs({c0, c0});
    auto *i3 = instrs.createADD({});
    i3->push_inputs({c0, c0});
    auto *i4 = instrs.createADD({});
    i4->push_inputs({c0, c0});
    auto *i5 = instrs.createADD({});
    i5->push_inputs({c0, c0});
    
    auto check1 = instrs.createCheck(IR::OpcodeType::CHECK1);
    check1->push_input(i1);
    auto check2 = instrs.createCheck(IR::OpcodeType::CHECK1);
    check2->push_input(i1); 
    auto check3 = instrs.createCheck(IR::OpcodeType::CHECK1);
    check3->push_input(i1);
    auto check4 = instrs.createCheck(IR::OpcodeType::CHECK1);
    check4->push_input(i1);  
    auto check5 = instrs.createCheck(IR::OpcodeType::CHECK1);
    check5->push_input(i1);   

    bb1->push_instrs({c0, c1});
    bb2->push_instrs({i1, check1});
    bb3->push_instrs({i2, check2});
    bb4->push_instrs({i3, check3});
    bb5->push_instrs({i4, check4, i5, check5});

    REQUIRE(g.add_node(*bb1) != G::KEY_UNDEF);
    REQUIRE(g.add_node(*bb2) != G::KEY_UNDEF);
    REQUIRE(g.add_node(*bb3) != G::KEY_UNDEF);
    REQUIRE(g.add_node(*bb4) != G::KEY_UNDEF);
    REQUIRE(g.add_node(*bb5) != G::KEY_UNDEF);

    REQUIRE(g.setHeader(bb1->get_id()) != G::KEY_UNDEF);

    REQUIRE(g.add_edge(0, 1, 2) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 2, 3) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 1, 4) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 4, 5) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 3, 5) != G::KEY_UNDEF);


    g.throwIfNotConsistent();
    instrs.throwIfNotConsistent_();


    std::cerr << g.dump();
    doCheckElimination(g);
    std::cerr << g.dump();

}