#!/usr/bin/env python

import random # for seed, random
import sys    # for stdout
import time


################################### TEST PART ##################################
################################################################################

# Tests align strands and scores
# Parameters types:
#    score          =  int   example: -6
#    plusScores     = string example: "  1   1  1"
#    minusScores    = string example: "22 111 11 "
#    strandAligned1 = string example: "  CAAGTCGC"
#    strandAligned2 = string example: "ATCCCATTAC"
#
#   Note: all strings must have same length
def test(score, plusScores, minusScores, strandAligned1, strandAligned2):
    print("\n>>>>>>START TEST<<<<<<")

    if testStrands(score, plusScores, minusScores, strandAligned1, strandAligned2):
        sys.stdout.write(">>>>>>>Test SUCCESS:")
        sys.stdout.write("\n\t\t" + "Score: "+str(score))
        sys.stdout.write("\n\t\t+ " + plusScores)
        sys.stdout.write("\n\t\t  " + strandAligned1)
        sys.stdout.write("\n\t\t  " + strandAligned2)
        sys.stdout.write("\n\t\t- " + minusScores)
        sys.stdout.write("\n\n")
    else:
        sys.stdout.write("\t>>>>!!!Test FAILED\n\n")


# converts character score to int
def testScoreToInt(score):
    if score == ' ':
        return 0
    return int(score)


# computes sum of scores
def testSumScore(scores):
    result = 0
    for ch in scores:
        result += testScoreToInt(ch)
    return result


# test each characters and scores
def testValidateEach(ch1, ch2, plusScore, minusScore):
    if ch1 == ' ' or ch2 == ' ':
        return plusScore == 0 and minusScore == 2
    if ch1 == ch2:
        return plusScore == 1 and minusScore == 0
    return plusScore == 0 and minusScore == 1


# test and validates strands
def testStrands(score, plusScores, minusScores, strandAligned1, strandAligned2):
    if len(plusScores) != len(minusScores) or len(minusScores) != len(strandAligned1) or len(strandAligned1) != len(
            strandAligned2):
        sys.stdout.write("Length mismatch! \n")
        return False

    if len(plusScores) == 0:
        sys.stdout.write("Length is Zero! \n")
        return False

    if testSumScore(plusScores) - testSumScore(minusScores) != score:
        sys.stdout.write("Score mismatch to score strings! TEST FAILED!\n")
        return False
    for i in range(len(plusScores)):
        if not testValidateEach(strandAligned1[i], strandAligned2[i], testScoreToInt(plusScores[i]),
                                testScoreToInt(minusScores[i])):
            sys.stdout.write("Invalid scores for position " + str(i) + ":\n")
            sys.stdout.write("\t char1: " + strandAligned1[i] + " char2: " +
                             strandAligned2[i] + " +" + str(testScoreToInt(plusScores[i])) + " -" +
                             str(testScoreToInt(minusScores[i])) + "\n")
            return False

    return True

######################## END OF TEST PART ######################################
################################################################################

def addToCache(strand1, strand2, cache, value):
    key=strand1+"//"+strand2
    cache[key]=value

def retAsDic(strand1,strand2,score):
    return {"strand1":strand1,"strand2":strand2, "score":score }

# Computes the score of the optimal alignment of two DNA strands.
def findOptimalAlignment(align, cache):

    strand1 = align["strand1"]
    strand2 = align["strand2"]
    score = align["score"]

    # if one of the two strands is empty, then there is only
    # one possible alignment, and of course it's optimal
    if len(strand1) == 0:
        return retAsDic(len(strand2) * " ",strand2,len(strand2) * -2)
    if len(strand2) == 0:
        return retAsDic(strand1,len(strand1) * " ",len(strand1) * -2)

    # There's the scenario where the two leading bases of
    # each strand are forced to align, regardless of whether or not
    # they actually match.
    key=strand1[1:]+"//"+strand2[1:]
    if cache.has_key(key):
        bestWith=cache[key]
    else:
        bestWith = findOptimalAlignment(retAsDic(strand1[1:], strand2[1:],score),cache)
        cache[key]=bestWith
    resultStrand1=strand1[0]+bestWith["strand1"]
    resultStrand2=strand2[0]+bestWith["strand2"]
    if strand1[0] == strand2[0]:
        return retAsDic(resultStrand1, resultStrand2,bestWith["score"] + 1) # no benefit from making other recursive calls

    best = bestWith
    best["score"]=best["score"]-1

    # It's possible that the leading base of strand1 best
    # matches not the leading base of strand2, but the one after it.
    key=strand1+"//"+strand2[1:]
    if cache.has_key(key):
        bestWithout=cache[key]
    else:
        bestWithout = findOptimalAlignment(retAsDic(strand1, strand2[1:],score),cache)
        bestWithout["score"] -= 2 # penalize for insertion of space
        cache[key]=bestWithout
    if bestWithout["score"] > best["score"]:
        resultStrand1=" "+bestWithout["strand1"]
        resultStrand2=strand2[0]+bestWithout["strand2"]
        best = bestWithout

    # opposite scenario
    key=strand1[1:]+"//"+strand2
    if cache.has_key(key):
        bestWithout=cache[key]
    else:
        bestWithout = findOptimalAlignment(retAsDic(strand1[1:], strand2,score),cache)
        bestWithout["score"] -= 2 # penalize for insertion of space
        cache[key]=bestWithout
    if bestWithout["score"] > best["score"]:
        resultStrand1=strand1[0]+bestWithout["strand1"]
        resultStrand2=" "+bestWithout["strand2"]
        best = bestWithout

    return retAsDic(resultStrand1, resultStrand2,best["score"])

# Utility function that generates a random DNA string of
# a random length drawn from the range [minlength, maxlength]
def generateRandomDNAStrand(minlength, maxlength):
    assert minlength > 0, \
           "Minimum length passed to generateRandomDNAStrand" \
           "must be a positive number" # these \'s allow mult-line statements
    assert maxlength >= minlength, \
           "Maximum length passed to generateRandomDNAStrand must be at " \
           "as large as the specified minimum length"
    strand = ""
    length = random.choice(xrange(minlength, maxlength + 1))
    bases = ['A', 'T', 'G', 'C']
    for i in xrange(0, length):
        strand += random.choice(bases)
    return strand

# Method that just prints out the supplied alignment score.
# This is more of a placeholder for what will ultimately
# print out not only the score but the alignment as well.

def printAlignment(alignment, out = sys.stdout):
    strand1 = alignment["strand1"]
    strand2 = alignment["strand2"]
    resultPlusString = ""
    resultMinusString = ""

    for i in range(0, len(strand1)):#doesn't matter wich one len(DNA1) = len(DNA2)
        if strand1[i]==strand2[i]:
            resultPlusString += "1"
            resultMinusString += " "
        elif strand1[i]== " " or strand2[i] == " ":
            resultPlusString += " "
            resultMinusString += "2"
        else:
            resultPlusString += " "
            resultMinusString += "1"


    out.write("Optimal alignment score is " + str(alignment["score"]) + "\n\n" +
          "   -  " + resultMinusString + "\n" +
          "      " + strand1 + "\n" +
          "      " + strand2 + "\n" +
          "   +  " + resultPlusString + "\n\n")


# Unit test main in place to do little more than
# exercise the above algorithm.  As written, it
# generates two fairly short DNA strands and
# determines the optimal alignment score.
#
# As you change the implementation of findOptimalAlignment
# to use memoization, you should change the 8s to 40s and
# the 10s to 60s and still see everything execute very
# quickly.

def main():
    cache={}
    while (True):
        sys.stdout.write("Generate random DNA strands? ")
        answer = sys.stdin.readline()
        if answer == "no\n": break
        strand1 = generateRandomDNAStrand(30,40)
        strand2 = generateRandomDNAStrand(30,40)
        sys.stdout.write("Aligning these two strands: " + strand1 + "\n")
        sys.stdout.write("                            " + strand2 + "\n")
        start = time.time()
        alignment = findOptimalAlignment({'strand1':"CAATGACAGAAAATCGGAGTCCCCTTGGTTACTGACTCTGAGGTAAGCATGGGGTAGAGCGAGGGTGGGTACTGGCGGGTCCAACAAAAGTGTTAATCACCAAGTGTCCCTATGAAAAAGACTACGTCTATTAGCTCCACGATCTTACTGCTAGCTTCACGTTATCTTGAAGCGTTACGGACAG",'strand2':"GATCAAGAGCCAGAATATGTGCTCTATCTGAGTGGGGTAAGTCATGACCGGGACTTAGTACTGCTTTTTCTCTCGTAGATCCCCCCCTAGTATGCGCGAGATCCTTAGTAACGCTCCATGTACGAGTGTGGTCTGGCGATTTGGGTATTATAACAGCACCCTGCTAGAGAGTGTAGAGAGACGTCTGGGACAA", 'score':0 }, cache)
        end = time.time()
        print(end - start)
        printAlignment(alignment)
        strand1 = alignment["strand1"]
        strand2 = alignment["strand2"]
        resultPlusString = ""
        resultMinusString = ""

        for i in range(0, len(strand1)):#doesn't matter wich one len(DNA1) = len(DNA2)
            if strand1[i]==strand2[i]:
                resultPlusString += "1"
                resultMinusString += " "
            elif strand1[i]== " " or strand2[i] == " ":
                resultPlusString += " "
                resultMinusString += "2"
            else:
                resultPlusString += " "
                resultMinusString += "1"

        test(alignment["score"], resultPlusString, resultMinusString, alignment["strand1"], alignment["strand2"])

if __name__ == "__main__":
  main()
