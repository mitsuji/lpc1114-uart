(define var1 '(1 2 3 4) )
(set! var1 '(2 3 4 5) )

(define lmd1 (lambda (x) (print (+ x 1))) )
(set! lmd1 (lambda (x) (print (+ x 2))) )

(defun lmd1 (lambda (x) (+ x 1)) )



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
