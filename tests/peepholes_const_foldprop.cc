#define CATCH_CONFIG_MAIN
#include "bbGraph.hpp"
#include "catch.hpp"
// #include "LoopTreeBuilder.hpp"
#include "constFoldProp.hpp"
#include "memManager.hpp"

/*
head:
    c1 = 1, c2 = 3, c3 = 3
bb1:
    a = c1 + c2
    b = a + c3

== after ==
head:
    c1 = 1, c2 = 3, c3 = 3, c4 = 4, c5 = 7
bb1:
    empty
*/
TEST_CASE("Test constfold", "[constfold]") {
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
    

    REQUIRE(g.add_node(*bb0) != G::KEY_UNDEF);
    REQUIRE(g.add_node(*bb1) != G::KEY_UNDEF);
    g.setHeader(bb0->get_id());
    std::cerr << g.dump();

    doConstFoldProp(g);

    std::cerr << g.dump();
}

/*
Atomic peepholes
== before ==
head:
    cx = 0, c0 = 1
bb1:
    i0 = load x // non-const instr
    i1 = i0 + i0
    i2 = i0 & 0
    i3 = i0 ^ i0
== after ==
head:
    cx = 0, c0 = 1, c1 = 0, c2 = 1, c3 = 0
bb1:
    i0 = load x
    i1 = i0 << 1 
*/
TEST_CASE("Test peepholes 1", "[peepholes1]") {
    IR::BbGraph g{};
    IR::BasicBlockManager bbs{};
    IR::InstrManager instrs{};
    auto *bb0 = bbs.create();
    auto *bb1 = bbs.create();
    
    auto *c0 = instrs.createCONST(IR::ValueHolder(0, IR::NO_VALUEHOLDER));
    auto *c1 = instrs.createCONST(IR::ValueHolder(1, IR::NO_VALUEHOLDER));
    auto *load = instrs.createLOAD({4, 1});

    auto i0 = instrs.createADD({});
    i0->push_inputs({load, load});
    auto i1 = instrs.createAND({});
    i1->push_inputs({i0, c0});
    auto i2 = instrs.createXOR({});
    i2->push_inputs({i0, i0});

    bb0->push_instrs({c0, c1});
    bb1->push_instrs({load, i0, i1, i2});

    bb0->throwIfNotConsistent_();
    bb1->throwIfNotConsistent_();

    REQUIRE(g.add_node(*bb0) != G::KEY_UNDEF);
    REQUIRE(g.add_node(*bb1) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(1, 1, 2) != G::KEY_UNDEF);
    g.setHeader(bb0->get_id());
    std::cerr << g.dump();

    doAtomicPeepholes(g, instrs);

    std::cerr << g.dump();
}

/*
Classic peephole
== before ==
head:
    cx = 0, c0 = 1, c1 = 2
bb1:
    i0 = load x // non-const instr
    i1 = i0 + cx
    i2 = i1 + c0
    i3 = i2 + c1
== after ==
head:
    c2 = 3
bb1:
    i0 = load x
    i3 = i0 + c2 
*/
TEST_CASE("Test peepholes 2", "[peepholes2]") {
    IR::BbGraph g{};
    IR::BasicBlockManager bbs{};
    IR::InstrManager instrs{};
    auto *bb0 = bbs.create();
    auto *bb1 = bbs.create();
    
    auto *cx = instrs.createCONST(IR::ValueHolder(0, IR::NO_VALUEHOLDER));
    auto *c0 = instrs.createCONST(IR::ValueHolder(1, IR::NO_VALUEHOLDER));
    auto *c1 = instrs.createCONST(IR::ValueHolder(2, IR::NO_VALUEHOLDER));

    auto load = instrs.createLOAD({4, 1});
    auto i0 = instrs.createADD({});
    i0->push_inputs({load, cx});
    auto i1 = instrs.createADD({});
    i1->push_inputs({i0, c0});
    auto i2 = instrs.createADD({});
    i2->push_inputs({i1, c1});

    bb0->push_instrs({cx, c0, c1});
    bb1->push_instrs({load, i0, i1, i2,});

    bb0->throwIfNotConsistent_();
    bb1->throwIfNotConsistent_();
    

    REQUIRE(g.add_node(*bb0) != G::KEY_UNDEF);
    REQUIRE(g.add_node(*bb1) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(1, 1, 2) != G::KEY_UNDEF);

    g.setHeader(bb0->get_id());
    std::cerr << g.dump();

    doPeepholes(g, instrs);

    std::cerr << g.dump();
}