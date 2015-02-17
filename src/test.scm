;(define rec
;    (lambda (a)
;        (if (<= 0 a)
;            0
;            (rec (- a 1)))))
;
;(rec 12)


; (if (= 1 1)
;     1
;     0)

; (if (> 2 3)
;     (+ 2 3)
;     2)

(+ 1 -2)

;(+ 2 1.4)

(define i #f)

(define y 10)
; (+ 1 (+ 2 3))

(+ 2 y)

; (define x y)
; (+ x y)

; (define st_hello "hello")
; (define string_hello st_hello)



; (define a (quote (y 3.4"hello")))

; (define f
;   (lambda (a b)
;         (+ a b)))

; (f 3 2)


;; (define g
;;   (lambda (a b)
;;         (- a b)))

;; (define z (quote (y "hello")))
;; (define zl (quote (y "hello" x 3 4.2 true)))

;; (define zw (quote (2 3 4 5)))


;; (f 20 y)




; (cdr z)

; (define c (cons 1 2 3))

; (if (= y 4)
;      (+ 3 4)
;      (- y 4))

; (x 1 (+ 1 2))

; (define ff-time
;     (lambda (vy)
;         (/ (* 2 vy) 9)))

;(/ 5 2.0)
;(- 1 2)
; (define pi 3)

; (define radian
;     (lambda (deg)
;         (* deg (/ pi 180))))


; (define dx
;     (lambda (vx t)
;         (* vx t)))



; (define deriv
;     (lambda (f dx)
;         (lambda (x)
;             (/
;                 (-
;                     (f (+ x dx))
;                     (f x))
;                 dx))))

; (define test
;     (lambda (a)
;         (lambda (c)
;             (a 1 c))))

; ((lambda (x) (+ x 1)) 2)

; (+ 1 2)
; error is occured by below code.
; ((deriv radian y) pi)
