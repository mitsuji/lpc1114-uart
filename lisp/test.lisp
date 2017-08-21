(define var1 '(1 2 3 4) )
(set! var1 '(2 3 4 5) )

(define lmd1 (lambda (x) (print (+ x 1))) )
(set! lmd1 (lambda (x) (print (+ x 2))) )

(defun lmd1 (lambda (x) (+ x 1)) )

(quote 1)
(quote (1 2))
(quote (1 2 3))
(quote a)
(quote (a b))
(quote (a b c))

(if t 100 200)
(if nil 100 200)
(if () 100 200)
(if (= 1 1) 100 200)
(if (= 0 1) 100 200)

((lambda (x) (+ x 1)) 1)
((lambda (x) (+ x 1)) 2)
((lambda (x) (+ x 1)) 100)

((lambda (x y) (+ x y)) 1 2)
((lambda (x y) (+ x y)) 2 3)
((lambda (x y) (+ x y)) 100 101)

(
 (
  (lambda (x)
    (lambda (y) (+ x y))
    )
  1)
 2)

(
 (
  (lambda (x)
    (lambda (y) (+ x y))
    )
  2)
 3)

(
 (
  (lambda (x)
    (lambda (y) (+ x y))
    )
  100)
 101)


(begin
 (print 'Alpha)
 (print 'Bravo)
 (print 'Charl)
 )


(cons 1 (cons 2 nil))
(cons 1 (cons 2 (cons 3 nil)))

(car (cons 1 (cons 2 nil)))
(cdr (cons 1 (cons 2 (cons 3 nil))))

(atom 1)
(atom '(1 2 3 4))
(atom (car '(1 2 3 4)))
(atom (cdr '(1 2 3 4)))

(atom 'a)
(atom '(a b c d))
(atom (car '(a b c d)))
(atom (cdr '(a b c d)))

(eq t t)
(eq nil nil)
(eq () ())
(eq t nil)
(eq t ())
(eq nil ())

(not t)
(not nil)
(not ())
(not (= 1 1))
(not (= 1 2))


(define f1
  (lambda (x y)
    (+ (* x x) (* y y))
    )
  )



(define for1
  (lambda (xs)
    (if (atom xs)
	()
      (begin
       (print (car xs))
       (for1 (cdr xs))
       )
      )
    )
  )
(for1 '(1 2 3 4))

(define while1
  (lambda (x)
    (if (< x 70)
	(begin
	 (print x)
	 (while1 (+ x 1))
	 )
      )
    )
  )
(while1 1)


(define while2
  (lambda (x)
    (begin
     (print x)
     (if (< x 70) (while2 (+ x 1)))
     )
    )
  )
(while2 1)


(define while3
  (lambda (x)
    (if (< x 40)
	(begin
	 (print x (% x 7))
	 (while3 (+ x 1))
	 )
      )
    )
  )
(while3 0)


