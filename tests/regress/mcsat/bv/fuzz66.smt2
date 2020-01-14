(set-logic QF_BV)
(declare-fun x () (_ BitVec 2))
(declare-fun y () (_ BitVec 2))
(declare-fun z () (_ BitVec 2))
(declare-fun w () (_ BitVec 2))
(declare-fun a () Bool)
(declare-fun b () Bool)
(assert 
    (and 
      (= z (_ bv0 2)) 
      (= y (_ bv0 2))
      (ite b (= w (_ bv0 2)) (= w z))
      (ite a (= x (bvadd w y)) (= x (_ bv0 2)))  
      (not (= x (_ bv0 2)))
    )
)
(check-sat)
(exit)
