(set-option :print-success false)
(set-logic QF_NRA)
(declare-fun skoZ () Real)
(declare-fun skoY () Real)
(declare-fun skoX () Real)

(push 2)
  (assert
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x252444 (<= skoZ ?x281453)))
   (let (($x281452 (and $x252444 $x286477)))
   (let (($x292302 (<= skoZ 1.0)))
   (let (($x253168 (and $x292302 $x281452)))
   (let (($x286489 (<= skoY 1.0)))
   (let (($x278872 (and $x286489 $x253168)))
   (let (($x281551 (<= skoX 1.0)))
   (let (($x286440 (and $x281551 $x278872)))
   (let (($x281450 (<= 0.0 skoZ)))
   (let (($x286790 (and $x281450 $x286440)))
   (let (($x289486 (<= 0.0 skoY)))
   (let (($x252597 (and $x289486 $x286790)))
   (let (($x282973 (and $x281450 $x252597)))
   (not $x282973)))))))))))))))))
  (assert
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x252444 (<= skoZ ?x281453)))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x281452 (and $x252444 $x286477)))
   (let (($x292302 (<= skoZ 1.0)))
   (let (($x253168 (and $x292302 $x281452)))
   (let (($x286489 (<= skoY 1.0)))
   (let (($x278872 (and $x286489 $x253168)))
   (let (($x281551 (<= skoX 1.0)))
   (let (($x286440 (and $x281551 $x278872)))
   (let (($x281450 (<= 0.0 skoZ)))
   (let (($x286790 (and $x281450 $x286440)))
   (let (($x281357 (not $x286790)))
   (let (($x292693 (and $x281357 $x252444)))
   (let (($x252706 (not $x292693)))
   (let (($x252952 (not $x252706)))
   (let (($x283083 (not $x252952)))
   (not $x283083)))))))))))))))))))
(check-sat)
(pop 1)

(push 2)
  (assert
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x252444 (<= skoZ ?x281453)))
   (let (($x281452 (and $x252444 $x286477)))
   (let (($x292302 (<= skoZ 1.0)))
   (let (($x253168 (and $x292302 $x281452)))
   (let (($x286489 (<= skoY 1.0)))
   (let (($x278872 (and $x286489 $x253168)))
   (let (($x281551 (<= skoX 1.0)))
   (let (($x286440 (and $x281551 $x278872)))
   (let (($x281450 (<= 0.0 skoZ)))
   (let (($x286790 (and $x281450 $x286440)))
   (let (($x289486 (<= 0.0 skoY)))
   (let (($x252597 (and $x289486 $x286790)))
   (let (($x284992 (and $x252597 $x286477)))
   (let (($x281357 (not $x286790)))
   (let (($x292693 (and $x281357 $x252444)))
   (let (($x252706 (not $x292693)))
   (let (($x252952 (not $x252706)))
   (let (($x282714 (and $x252952 $x284992)))
   (let (($x278988 (<= 0.0 skoX)))
   (let (($x284181 (and $x278988 $x252597)))
   (let ((?x281387 (+ (- 840.0) (* skoZ (+ 180.0 (* skoZ (+ (- 20.0) skoZ)))))))
   (let (($x281559 (<= (* skoZ ?x281387) (- 1680.0))))
   (let (($x252464 (and $x281559 $x284181)))
   (let (($x252950 (and $x252464 $x282714)))
   (not $x252950))))))))))))))))))))))))))))
  (assert
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x252444 (<= skoZ ?x281453)))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x281452 (and $x252444 $x286477)))
   (let (($x292302 (<= skoZ 1.0)))
   (let (($x253168 (and $x292302 $x281452)))
   (let (($x286489 (<= skoY 1.0)))
   (let (($x278872 (and $x286489 $x253168)))
   (let (($x281551 (<= skoX 1.0)))
   (let (($x286440 (and $x281551 $x278872)))
   (let (($x281450 (<= 0.0 skoZ)))
   (let (($x286790 (and $x281450 $x286440)))
   (let (($x281357 (not $x286790)))
   (let (($x292693 (and $x281357 $x252444)))
   (let (($x252584 (and $x292693 $x292693)))
   (and $x292302 $x252584)))))))))))))))))
(check-sat)
(pop 1)

(push 2)
  (assert
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x289486 (<= 0.0 skoY)))
   (let (($x252453 (and $x289486 $x286477)))
   (let (($x286995 (not $x252453)))
   (let (($x252444 (<= skoZ ?x281453)))
   (let (($x281452 (and $x252444 $x286477)))
   (let (($x292302 (<= skoZ 1.0)))
   (let (($x253168 (and $x292302 $x281452)))
   (let (($x286489 (<= skoY 1.0)))
   (let (($x278872 (and $x286489 $x253168)))
   (let (($x281551 (<= skoX 1.0)))
   (let (($x286440 (and $x281551 $x278872)))
   (let (($x281450 (<= 0.0 skoZ)))
   (let (($x286790 (and $x281450 $x286440)))
   (let (($x252597 (and $x289486 $x286790)))
   (let (($x289560 (and $x252597 $x252444)))
   (let (($x290061 (not $x289560)))
   (let (($x278989 (and $x252444 $x290061)))
   (let (($x294302 (and $x278989 $x286995)))
   (let (($x289915 (not $x294302)))
   (not $x289915)))))))))))))))))))))))
  (assert
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x289486 (<= 0.0 skoY)))
   (let (($x252453 (and $x289486 $x286477)))
   (not $x252453))))))
(check-sat)
(pop 2)

(push 2)
  (assert
   (let (($x281450 (<= 0.0 skoZ)))
   (let (($x286456 (not $x281450)))
   (let (($x277517 (not $x286456)))
   (let (($x281490 (not $x277517)))
   (not $x281490))))))
  (assert
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x252444 (<= skoZ ?x281453)))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x281452 (and $x252444 $x286477)))
   (let (($x292302 (<= skoZ 1.0)))
   (let (($x253168 (and $x292302 $x281452)))
   (let (($x286489 (<= skoY 1.0)))
   (let (($x278872 (and $x286489 $x253168)))
   (let (($x281551 (<= skoX 1.0)))
   (let (($x286440 (and $x281551 $x278872)))
   (let (($x281450 (<= 0.0 skoZ)))
   (let (($x286790 (and $x281450 $x286440)))
   (let (($x281357 (not $x286790)))
   (let (($x292693 (and $x281357 $x252444)))
   (let (($x252706 (not $x292693)))
   (let (($x252952 (not $x252706)))
   (let (($x289486 (<= 0.0 skoY)))
   (let (($x252597 (and $x289486 $x286790)))
   (let (($x282973 (and $x281450 $x252597)))
   (let (($x253065 (and $x282973 $x252952)))
   (let (($x134521 (not $x253065)))
   (let (($x252394 (not $x134521)))
   (not $x252394))))))))))))))))))))))))
(check-sat)
(pop 0)

(push 2)
  (assert
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x252444 (<= skoZ ?x281453)))
   (let (($x281452 (and $x252444 $x286477)))
   (let (($x292302 (<= skoZ 1.0)))
   (let (($x253168 (and $x292302 $x281452)))
   (let (($x286489 (<= skoY 1.0)))
   (let (($x278872 (and $x286489 $x253168)))
   (let (($x281551 (<= skoX 1.0)))
   (let (($x286440 (and $x281551 $x278872)))
   (let (($x273359 (not $x286440)))
   (let (($x281450 (<= 0.0 skoZ)))
   (let (($x286790 (and $x281450 $x286440)))
   (let (($x289486 (<= 0.0 skoY)))
   (let (($x252597 (and $x289486 $x286790)))
   (let (($x289560 (and $x252597 $x252444)))
   (let (($x290061 (not $x289560)))
   (let (($x278989 (and $x252444 $x290061)))
   (and $x278989 $x273359))))))))))))))))))))
  (assert
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x252444 (<= skoZ ?x281453)))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x281452 (and $x252444 $x286477)))
   (let (($x292302 (<= skoZ 1.0)))
   (let (($x253168 (and $x292302 $x281452)))
   (let (($x286489 (<= skoY 1.0)))
   (let (($x278872 (and $x286489 $x253168)))
   (let (($x281551 (<= skoX 1.0)))
   (let (($x286440 (and $x281551 $x278872)))
   (let (($x281450 (<= 0.0 skoZ)))
   (let (($x286790 (and $x281450 $x286440)))
   (let (($x281357 (not $x286790)))
   (let (($x292693 (and $x281357 $x252444)))
   (let (($x252706 (not $x292693)))
   (let (($x252952 (not $x252706)))
   (let (($x283083 (not $x252952)))
   (not $x283083)))))))))))))))))))
(check-sat)
(pop 0)

(push 2)
  (assert
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x252444 (<= skoZ ?x281453)))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x281452 (and $x252444 $x286477)))
   (let (($x292302 (<= skoZ 1.0)))
   (let (($x253168 (and $x292302 $x281452)))
   (let (($x286489 (<= skoY 1.0)))
   (let (($x278872 (and $x286489 $x253168)))
   (let (($x281551 (<= skoX 1.0)))
   (let (($x286440 (and $x281551 $x278872)))
   (let (($x281450 (<= 0.0 skoZ)))
   (let (($x286790 (and $x281450 $x286440)))
   (let (($x281357 (not $x286790)))
   (let (($x292693 (and $x281357 $x252444)))
   (let (($x252706 (not $x292693)))
   (let (($x252952 (not $x252706)))
   (let (($x289486 (<= 0.0 skoY)))
   (let (($x252597 (and $x289486 $x286790)))
   (let (($x282973 (and $x281450 $x252597)))
   (let (($x253065 (and $x282973 $x252952)))
   (let (($x252940 (not $x282973)))
   (let (($x252570 (and $x252940 $x253065)))
   (let (($x286456 (not $x281450)))
   (let (($x252456 (and $x286456 $x252570)))
   (not $x252456))))))))))))))))))))))))))
  (assert
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x289486 (<= 0.0 skoY)))
   (let (($x252453 (and $x289486 $x286477)))
   (let (($x286995 (not $x252453)))
   (let (($x252444 (<= skoZ ?x281453)))
   (let (($x281452 (and $x252444 $x286477)))
   (let (($x292302 (<= skoZ 1.0)))
   (let (($x253168 (and $x292302 $x281452)))
   (let (($x286489 (<= skoY 1.0)))
   (let (($x278872 (and $x286489 $x253168)))
   (let (($x281551 (<= skoX 1.0)))
   (let (($x286440 (and $x281551 $x278872)))
   (let (($x281450 (<= 0.0 skoZ)))
   (let (($x286790 (and $x281450 $x286440)))
   (let (($x252597 (and $x289486 $x286790)))
   (let (($x289560 (and $x252597 $x252444)))
   (let (($x290061 (not $x289560)))
   (let (($x278989 (and $x252444 $x290061)))
   (let (($x294302 (and $x278989 $x286995)))
   (let (($x289915 (not $x294302)))
   (not $x289915)))))))))))))))))))))))
(check-sat)
(pop 0)

(push 2)
  (assert
   (let (($x292302 (<= skoZ 1.0)))
   (let (($x278988 (<= 0.0 skoX)))
   (let (($x283778 (and $x278988 $x292302)))
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x252444 (<= skoZ ?x281453)))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x281452 (and $x252444 $x286477)))
   (let (($x253168 (and $x292302 $x281452)))
   (let (($x286489 (<= skoY 1.0)))
   (let (($x278872 (and $x286489 $x253168)))
   (let (($x281551 (<= skoX 1.0)))
   (let (($x286440 (and $x281551 $x278872)))
   (let (($x281450 (<= 0.0 skoZ)))
   (let (($x286790 (and $x281450 $x286440)))
   (let (($x281357 (not $x286790)))
   (let (($x292693 (and $x281357 $x252444)))
   (let (($x252584 (and $x292693 $x292693)))
   (let (($x289486 (<= 0.0 skoY)))
   (let (($x252984 (and $x289486 $x252584)))
   (let (($x252440 (and $x252984 $x252444)))
   (let (($x289785 (and $x252440 $x283778)))
   (not $x289785)))))))))))))))))))))))
  (assert
   (let (($x292302 (<= skoZ 1.0)))
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x252444 (<= skoZ ?x281453)))
   (let (($x281452 (and $x252444 $x286477)))
   (let (($x253168 (and $x292302 $x281452)))
   (let (($x286489 (<= skoY 1.0)))
   (let (($x278872 (and $x286489 $x253168)))
   (let (($x279854 (and $x286477 $x278872)))
   (let (($x281451 (and $x279854 $x292302)))
   (let (($x281551 (<= skoX 1.0)))
   (let (($x286440 (and $x281551 $x278872)))
   (let (($x281450 (<= 0.0 skoZ)))
   (let (($x286790 (and $x281450 $x286440)))
   (let (($x289486 (<= 0.0 skoY)))
   (let (($x252597 (and $x289486 $x286790)))
   (let (($x289560 (and $x252597 $x252444)))
   (let (($x290061 (not $x289560)))
   (let (($x284992 (and $x252597 $x286477)))
   (let (($x281357 (not $x286790)))
   (let (($x292693 (and $x281357 $x252444)))
   (let (($x252706 (not $x292693)))
   (let (($x252952 (not $x252706)))
   (let (($x282714 (and $x252952 $x284992)))
   (let (($x281388 (and $x282714 $x290061)))
   (let (($x253089 (and $x281388 $x281451)))
   (not $x253089))))))))))))))))))))))))))))
(check-sat)
(pop 1)

(push 2)
  (assert
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x252444 (<= skoZ ?x281453)))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x281452 (and $x252444 $x286477)))
   (let (($x292302 (<= skoZ 1.0)))
   (let (($x253168 (and $x292302 $x281452)))
   (let (($x286489 (<= skoY 1.0)))
   (let (($x278872 (and $x286489 $x253168)))
   (let (($x281551 (<= skoX 1.0)))
   (let (($x286440 (and $x281551 $x278872)))
   (let (($x281450 (<= 0.0 skoZ)))
   (let (($x286790 (and $x281450 $x286440)))
   (let (($x281357 (not $x286790)))
   (let (($x292693 (and $x281357 $x252444)))
   (let (($x252706 (not $x292693)))
   (let (($x252952 (not $x252706)))
   (let (($x283083 (not $x252952)))
   (not $x283083)))))))))))))))))))
  (assert
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x252444 (<= skoZ ?x281453)))
   (let (($x281452 (and $x252444 $x286477)))
   (let (($x292302 (<= skoZ 1.0)))
   (let (($x253168 (and $x292302 $x281452)))
   (let (($x286489 (<= skoY 1.0)))
   (let (($x278872 (and $x286489 $x253168)))
   (let (($x286455 (not $x278872)))
   (let (($x281551 (<= skoX 1.0)))
   (let (($x286440 (and $x281551 $x278872)))
   (let (($x281450 (<= 0.0 skoZ)))
   (let (($x286790 (and $x281450 $x286440)))
   (let (($x289486 (<= 0.0 skoY)))
   (let (($x252597 (and $x289486 $x286790)))
   (let (($x278988 (<= 0.0 skoX)))
   (let (($x284181 (and $x278988 $x252597)))
   (let ((?x281387 (+ (- 840.0) (* skoZ (+ 180.0 (* skoZ (+ (- 20.0) skoZ)))))))
   (let (($x281559 (<= (* skoZ ?x281387) (- 1680.0))))
   (let (($x252464 (and $x281559 $x284181)))
   (let (($x287728 (and $x252464 $x286455)))
   (let (($x281357 (not $x286790)))
   (let (($x292693 (and $x281357 $x252444)))
   (let (($x252584 (and $x292693 $x292693)))
   (let (($x252706 (not $x292693)))
   (let (($x252952 (not $x252706)))
   (let (($x283083 (not $x252952)))
   (let (($x280389 (and $x283083 $x252584)))
   (let (($x293838 (and $x280389 $x287728)))
   (not $x293838)))))))))))))))))))))))))))))))
(check-sat)
(pop 2)

(push 2)
  (assert
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x252444 (<= skoZ ?x281453)))
   (let (($x281452 (and $x252444 $x286477)))
   (let (($x292302 (<= skoZ 1.0)))
   (let (($x253168 (and $x292302 $x281452)))
   (let (($x286489 (<= skoY 1.0)))
   (let (($x278872 (and $x286489 $x253168)))
   (let (($x281551 (<= skoX 1.0)))
   (let (($x286440 (and $x281551 $x278872)))
   (let (($x273359 (not $x286440)))
   (and $x281551 $x273359)))))))))))))
  (assert
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x252444 (<= skoZ ?x281453)))
   (let (($x281452 (and $x252444 $x286477)))
   (let (($x292302 (<= skoZ 1.0)))
   (let (($x253168 (and $x292302 $x281452)))
   (let (($x286489 (<= skoY 1.0)))
   (let (($x278872 (and $x286489 $x253168)))
   (let (($x281551 (<= skoX 1.0)))
   (let (($x286440 (and $x281551 $x278872)))
   (let (($x281450 (<= 0.0 skoZ)))
   (let (($x286790 (and $x281450 $x286440)))
   (let (($x289486 (<= 0.0 skoY)))
   (let (($x252597 (and $x289486 $x286790)))
   (let (($x282973 (and $x281450 $x252597)))
   (not $x282973)))))))))))))))))
(check-sat)
(pop 2)

(push 2)
  (assert
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x252444 (<= skoZ ?x281453)))
   (let (($x281452 (and $x252444 $x286477)))
   (let (($x292302 (<= skoZ 1.0)))
   (let (($x253168 (and $x292302 $x281452)))
   (let (($x286489 (<= skoY 1.0)))
   (let (($x278872 (and $x286489 $x253168)))
   (let (($x281551 (<= skoX 1.0)))
   (let (($x286440 (and $x281551 $x278872)))
   (let (($x281450 (<= 0.0 skoZ)))
   (let (($x286790 (and $x281450 $x286440)))
   (let (($x289486 (<= 0.0 skoY)))
   (let (($x252597 (and $x289486 $x286790)))
   (let (($x282973 (and $x281450 $x252597)))
   (let (($x252940 (not $x282973)))
   (let (($x281357 (not $x286790)))
   (let (($x292693 (and $x281357 $x252444)))
   (let (($x252706 (not $x292693)))
   (let (($x252952 (not $x252706)))
   (and $x252952 $x252940))))))))))))))))))))))
  (assert
   (let (($x278988 (<= 0.0 skoX)))
   (let (($x252870 (not $x278988)))
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x289486 (<= 0.0 skoY)))
   (let (($x252453 (and $x289486 $x286477)))
   (let (($x286995 (not $x252453)))
   (let (($x252647 (and $x286995 $x252870)))
   (let (($x281450 (<= 0.0 skoZ)))
   (let (($x273211 (and $x281450 $x252647)))
   (not $x273211))))))))))))
(check-sat)
(pop 0)

(push 2)
  (assert
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x252444 (<= skoZ ?x281453)))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x281452 (and $x252444 $x286477)))
   (let (($x292302 (<= skoZ 1.0)))
   (let (($x253168 (and $x292302 $x281452)))
   (let (($x286489 (<= skoY 1.0)))
   (let (($x278872 (and $x286489 $x253168)))
   (let (($x281551 (<= skoX 1.0)))
   (let (($x286440 (and $x281551 $x278872)))
   (let (($x281450 (<= 0.0 skoZ)))
   (let (($x286790 (and $x281450 $x286440)))
   (let (($x281357 (not $x286790)))
   (let (($x292693 (and $x281357 $x252444)))
   (let (($x252706 (not $x292693)))
   (let (($x252952 (not $x252706)))
   (let (($x289486 (<= 0.0 skoY)))
   (let (($x252597 (and $x289486 $x286790)))
   (let (($x282973 (and $x281450 $x252597)))
   (let (($x253065 (and $x282973 $x252952)))
   (let (($x252940 (not $x282973)))
   (let (($x252570 (and $x252940 $x253065)))
   (let (($x286456 (not $x281450)))
   (let (($x252456 (and $x286456 $x252570)))
   (not $x252456))))))))))))))))))))))))))
  (assert
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x252444 (<= skoZ ?x281453)))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x281452 (and $x252444 $x286477)))
   (let (($x292302 (<= skoZ 1.0)))
   (let (($x253168 (and $x292302 $x281452)))
   (let (($x286489 (<= skoY 1.0)))
   (let (($x278872 (and $x286489 $x253168)))
   (let (($x281551 (<= skoX 1.0)))
   (let (($x286440 (and $x281551 $x278872)))
   (let (($x281450 (<= 0.0 skoZ)))
   (let (($x286790 (and $x281450 $x286440)))
   (let (($x281357 (not $x286790)))
   (let (($x292693 (and $x281357 $x252444)))
   (and $x292693 $x292693))))))))))))))))
(check-sat)
(pop 0)

(push 2)
  (assert
   (let ((?x281387 (+ (- 840.0) (* skoZ (+ 180.0 (* skoZ (+ (- 20.0) skoZ)))))))
   (let (($x281559 (<= (* skoZ ?x281387) (- 1680.0))))
   (let (($x286489 (<= skoY 1.0)))
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x252444 (<= skoZ ?x281453)))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x281452 (and $x252444 $x286477)))
   (let (($x292302 (<= skoZ 1.0)))
   (let (($x253168 (and $x292302 $x281452)))
   (let (($x278872 (and $x286489 $x253168)))
   (let (($x281551 (<= skoX 1.0)))
   (let (($x286440 (and $x281551 $x278872)))
   (let (($x281450 (<= 0.0 skoZ)))
   (let (($x286790 (and $x281450 $x286440)))
   (let (($x281357 (not $x286790)))
   (let (($x292693 (and $x281357 $x252444)))
   (let (($x252706 (not $x292693)))
   (let (($x281442 (and $x252706 $x286489)))
   (let (($x286669 (and $x281442 $x281559)))
   (not $x286669)))))))))))))))))))))
  (assert
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x252444 (<= skoZ ?x281453)))
   (let (($x281452 (and $x252444 $x286477)))
   (let (($x292302 (<= skoZ 1.0)))
   (let (($x253168 (and $x292302 $x281452)))
   (let (($x286489 (<= skoY 1.0)))
   (let (($x278872 (and $x286489 $x253168)))
   (let (($x281551 (<= skoX 1.0)))
   (let (($x286440 (and $x281551 $x278872)))
   (let (($x273359 (not $x286440)))
   (let (($x252718 (and $x281551 $x273359)))
   (let (($x270568 (and $x286440 $x292302)))
   (let (($x281450 (<= 0.0 skoZ)))
   (let (($x286790 (and $x281450 $x286440)))
   (let (($x281357 (not $x286790)))
   (let (($x292693 (and $x281357 $x252444)))
   (let (($x252706 (not $x292693)))
   (let (($x252952 (not $x252706)))
   (let (($x252979 (and $x252952 $x270568)))
   (let (($x286712 (and $x252979 $x252718)))
   (not $x286712)))))))))))))))))))))))
(check-sat)
(pop 0)

(push 2)
  (assert
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x286785 (not $x286477)))
   (let (($x286489 (<= skoY 1.0)))
   (and $x286489 $x286785))))))
  (assert
   (let (($x278988 (<= 0.0 skoX)))
   (let (($x281551 (<= skoX 1.0)))
   (let (($x252976 (and $x281551 $x278988)))
   (let (($x252975 (not $x252976)))
   (not $x252975))))))
(check-sat)
(pop 1)

(push 2)
  (assert
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x252444 (<= skoZ ?x281453)))
   (let (($x281452 (and $x252444 $x286477)))
   (let (($x292302 (<= skoZ 1.0)))
   (let (($x253168 (and $x292302 $x281452)))
   (not $x253168))))))))
  (assert
   (let (($x286489 (<= skoY 1.0)))
   (let (($x281551 (<= skoX 1.0)))
   (and $x281551 $x286489))))
(check-sat)
(pop 2)

(push 2)
  (assert
   (let (($x281551 (<= skoX 1.0)))
   (let (($x292302 (<= skoZ 1.0)))
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x252444 (<= skoZ ?x281453)))
   (let (($x281452 (and $x252444 $x286477)))
   (let (($x253168 (and $x292302 $x281452)))
   (let (($x286489 (<= skoY 1.0)))
   (let (($x278872 (and $x286489 $x253168)))
   (let (($x286440 (and $x281551 $x278872)))
   (let (($x270568 (and $x286440 $x292302)))
   (let (($x252703 (and $x270568 $x281551)))
   (let (($x286960 (not $x252703)))
   (let (($x252478 (not $x286960)))
   (not $x252478))))))))))))))))
  (assert
   (let (($x292302 (<= skoZ 1.0)))
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x252444 (<= skoZ ?x281453)))
   (let (($x281452 (and $x252444 $x286477)))
   (let (($x253168 (and $x292302 $x281452)))
   (let (($x286489 (<= skoY 1.0)))
   (let (($x278872 (and $x286489 $x253168)))
   (let (($x281551 (<= skoX 1.0)))
   (let (($x286440 (and $x281551 $x278872)))
   (let (($x270568 (and $x286440 $x292302)))
   (not $x270568)))))))))))))
(check-sat)
(pop 0)

(push 2)
  (assert
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x252444 (<= skoZ ?x281453)))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x281452 (and $x252444 $x286477)))
   (let (($x292302 (<= skoZ 1.0)))
   (let (($x253168 (and $x292302 $x281452)))
   (let (($x286489 (<= skoY 1.0)))
   (let (($x278872 (and $x286489 $x253168)))
   (let (($x281551 (<= skoX 1.0)))
   (let (($x286440 (and $x281551 $x278872)))
   (let (($x281450 (<= 0.0 skoZ)))
   (let (($x286790 (and $x281450 $x286440)))
   (let (($x281357 (not $x286790)))
   (let (($x292693 (and $x281357 $x252444)))
   (let (($x289486 (<= 0.0 skoY)))
   (let (($x252597 (and $x289486 $x286790)))
   (let (($x278988 (<= 0.0 skoX)))
   (let (($x284181 (and $x278988 $x252597)))
   (let ((?x281387 (+ (- 840.0) (* skoZ (+ 180.0 (* skoZ (+ (- 20.0) skoZ)))))))
   (let (($x281559 (<= (* skoZ ?x281387) (- 1680.0))))
   (let (($x252464 (and $x281559 $x284181)))
   (let (($x253084 (and $x252464 $x292693)))
   (not $x253084))))))))))))))))))))))))
  (assert
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x252444 (<= skoZ ?x281453)))
   (let (($x281452 (and $x252444 $x286477)))
   (let (($x292302 (<= skoZ 1.0)))
   (let (($x253168 (and $x292302 $x281452)))
   (let (($x286489 (<= skoY 1.0)))
   (let (($x278872 (and $x286489 $x253168)))
   (let (($x286455 (not $x278872)))
   (let (($x281551 (<= skoX 1.0)))
   (let (($x286440 (and $x281551 $x278872)))
   (let (($x281450 (<= 0.0 skoZ)))
   (let (($x286790 (and $x281450 $x286440)))
   (let (($x289486 (<= 0.0 skoY)))
   (let (($x252597 (and $x289486 $x286790)))
   (let (($x278988 (<= 0.0 skoX)))
   (let (($x284181 (and $x278988 $x252597)))
   (let ((?x281387 (+ (- 840.0) (* skoZ (+ 180.0 (* skoZ (+ (- 20.0) skoZ)))))))
   (let (($x281559 (<= (* skoZ ?x281387) (- 1680.0))))
   (let (($x252464 (and $x281559 $x284181)))
   (let (($x287728 (and $x252464 $x286455)))
   (let (($x281357 (not $x286790)))
   (let (($x292693 (and $x281357 $x252444)))
   (let (($x252584 (and $x292693 $x292693)))
   (let (($x252706 (not $x292693)))
   (let (($x252952 (not $x252706)))
   (let (($x283083 (not $x252952)))
   (let (($x280389 (and $x283083 $x252584)))
   (let (($x293838 (and $x280389 $x287728)))
   (not $x293838)))))))))))))))))))))))))))))))
(check-sat)
(pop 0)

(push 2)
  (assert
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x252444 (<= skoZ ?x281453)))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x281452 (and $x252444 $x286477)))
   (let (($x291373 (and $x281452 $x252444)))
   (not $x291373)))))))
  (assert
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x252444 (<= skoZ ?x281453)))
   (let (($x281452 (and $x252444 $x286477)))
   (let (($x292302 (<= skoZ 1.0)))
   (let (($x253168 (and $x292302 $x281452)))
   (let (($x286489 (<= skoY 1.0)))
   (let (($x278872 (and $x286489 $x253168)))
   (let (($x281551 (<= skoX 1.0)))
   (let (($x286440 (and $x281551 $x278872)))
   (let (($x281450 (<= 0.0 skoZ)))
   (let (($x286790 (and $x281450 $x286440)))
   (let (($x289486 (<= 0.0 skoY)))
   (let (($x252597 (and $x289486 $x286790)))
   (let (($x278988 (<= 0.0 skoX)))
   (let (($x284181 (and $x278988 $x252597)))
   (let ((?x281387 (+ (- 840.0) (* skoZ (+ 180.0 (* skoZ (+ (- 20.0) skoZ)))))))
   (let (($x281559 (<= (* skoZ ?x281387) (- 1680.0))))
   (let (($x252464 (and $x281559 $x284181)))
   (not $x252464)))))))))))))))))))))
(check-sat)
(pop 0)

(push 2)
  (assert
   (let (($x281551 (<= skoX 1.0)))
   (let (($x292302 (<= skoZ 1.0)))
   (let ((?x281453 (+ (+ 2.0 (* skoX (- 1.0))) (* skoY (- 1.0)))))
   (let (($x286477 (<= ?x281453 skoZ)))
   (let (($x252444 (<= skoZ ?x281453)))
   (let (($x281452 (and $x252444 $x286477)))
   (let (($x253168 (and $x292302 $x281452)))
   (let (($x286489 (<= skoY 1.0)))
   (let (($x278872 (and $x286489 $x253168)))
   (let (($x286440 (and $x281551 $x278872)))
   (let (($x270568 (and $x286440 $x292302)))
   (let (($x252703 (and $x270568 $x281551)))
   (let (($x286960 (not $x252703)))
   (let (($x252478 (not $x286960)))
   (not $x252478))))))))))))))))
  (assert
   (let (($x281551 (<= skoX 1.0)))
   (let (($x286658 (not $x281551)))
   (not $x286658))))
(check-sat)
