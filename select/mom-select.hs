module Main where

import List

group5 :: (Ord a) => [a] -> [[a]]
group5 [] = []
group5 xs = (take 5 xs) : group5 (drop 5 xs)

median :: (Ord a) => [a] -> a
median xs =
    let
        s = sort xs
        sz = length xs
    in
        s !! (sz `div` 2)

momPivot :: (Ord a) => [a] -> a
momPivot [x] = x
momPivot xs =
    let
        groups = group5 xs
        medians = map median groups
    in
        momPivot medians

kSmallest :: (Ord a) => Int -> [a] -> [a]
kSmallest k xs =
    let
        pivot = momPivot xs
        (s1,l1) = partition (<=pivot) (take k xs)
        (s2,l2) = partition (<pivot) (drop k xs)
        (s,l) = (s1++s2, l1++l2)
        slen = length s
    in
        if slen == k
        then s
        else
            if slen < k
            then s ++ (kSmallest (k-slen) l)
            else (kSmallest k s)
