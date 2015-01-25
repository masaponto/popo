
(+ 1 2)

(define i #f)

(define y 4)

(define st_hello "hello")
(define string_hello st_hello)
(define x
    (lambda (a b)
        (+ a b)))


(x 1 (+ 1 2))

(define z (quote (y "hello")))

(if (+ y 4)
     (+ y 4)
     (- y 4))

(define ff-time
    (lambda (vy)
        (/ (* 2 vy) 9)))
(/ 1 2)

(define pi 3)

(define radian (lambda (deg)
    (* deg (/ pi 180))))


(define dx
    (lambda (vx t)
        (* vx t)))

(define deriv 
    (lambda (f dx)
        (lambda (x)
            (/
                (-
                    (f (+ x dx))
                    (f x)
                dx)))))
