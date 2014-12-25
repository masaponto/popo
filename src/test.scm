(+ 1 2)

(define (bar x)
    (foo 0 x))

(let binds body)

(define (k+ a b k)
    (k (+ a b)))

#|
; definition of pi
(define pi (* 4 (atan 1)))

; degree -> radian
(define (radian deg)
    (* deg (/ pi 180)))

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

