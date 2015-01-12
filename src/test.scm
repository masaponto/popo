(+ 1 2)

(define y 4)

(define st_hello "hello")
(define string_hello st_hello)
(define x (lambda (a b) (+ a b)))

(define z (quote (y "hello")))

(if (+ y 4)
     (+ y 4)
     (- y 4))

#|

; definition of pi
(define pi (* 4 (atan 1)))

; degree -> radian
(define radian (lambda (deg)
    (* deg (/ pi 180))))

; free fall time
(define (ff-time vy)
    (/ (* 2 vy) 9))

; horizontal distance
(define (dx vx t)
    (* vx t))

; distance
(define (distance v ang)
    (dx
        (* v (cos (radian ang)))                ; vx
        (ff-time (* v (sin (radian ang))))))    ; t
|#

