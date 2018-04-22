#!/usr/bin/env python

import random # for seed, random
import sys    # for stdout
import operator


################################### TEST PART ##################################
################################################################################

def get_score_strings(strand1,strand2):
	plus=''
	minus=''
	for i in range(len(strand1)):
		if strand1[i]==strand2[i]:
			plus += "1"
			minus += " "
		elif strand1[i]== " " or strand2[i] == " ":
			plus += " "
			minus += "2"
		else:
			plus += " "
			minus += "1"
	return (plus, minus)

# Tests align strands and scores
# Parameters types:
#    score          =  int   example: -6
#    	--	plusScores     = string example: "  1   1  1"
#    	--	minusScores    = string example: "22 111 11 "
#    strandAligned1 = string example: "  CAAGTCGC"
#    strandAligned2 = string example: "ATCCCATTAC"
#
#   Note: all strings must have same length
def test(score, strandAligned1, strandAligned2):

	test_score=1 #es roca 1 aris maSin 10ze patara sigrZeebs imazec testavs, rac moyva da 0 rocaa gamorTulia eg testi

	plusScores,minusScores=get_score_strings(strandAligned1,strandAligned2)

	print("\n>>>>>>START TEST<<<<<<")

	if testStrands(score, plusScores, minusScores, strandAligned1, strandAligned2,test_score):
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

def foa(strand1, strand2):
	if len(strand1) == 0: return len(strand2) * -2
	if len(strand2) == 0: return len(strand1) * -2
	bestWith = foa(strand1[1:], strand2[1:])
	if strand1[0] == strand2[0]: 
		return bestWith + 1
	best = bestWith - 1
	bestWithout = foa(strand1, strand2[1:])
	bestWithout -= 2
	if bestWithout > best:
		best = bestWithout
	bestWithout = foa(strand1[1:], strand2)
	bestWithout -= 2
	if bestWithout > best:
		best = bestWithout
	return best

# test and validates strands
def testStrands(score, plusScores, minusScores, strandAligned1, strandAligned2, test_score):
	s1=strandAligned1.replace(' ','')
	s2=strandAligned2.replace(' ','')
	if test_score and len(s1)<15 and len(s2)<15:
		if score != foa(s1, s2):
			sys.stdout.write("Wrong Score! \n")

	if len(plusScores) != len(minusScores) or len(minusScores) != len(strandAligned1) or len(strandAligned1) != len(strandAligned2):
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


# Computes the score of the optimal alignment of two DNA strands.
def findOptimalAlignment(cache, strand1, strand2):
	#print(strand1,strand2)
	# if one of the two strands is empty, then there is only
	# one possible alignment, and of course it's optimal
	if len(strand1) == 0: return (len(strand2) * -2, len(strand2)*' ', strand2)
	if len(strand2) == 0: return (len(strand1) * -2, strand1, len(strand1)*' ')

	# There's the scenario where the two leading bases of
	# each strand are forced to align, regardless of whether or not
	# they actually match.
	bestWith, s1, s2 = findOptimalAlignment(cache, strand1[1:], strand2[1:]) if (strand1[1:], strand2[1:]) not in cache else cache[(strand1[1:], strand2[1:])]
	cache[(strand1[1:], strand2[1:])]=(bestWith, s1, s2)

	if strand1[0] == strand2[0]:
		return (bestWith+1, strand1[0]+s1, strand2[0]+s2) # no benefit from making other recursive calls
	
	scores = [(bestWith - 1, s1, s2)]
	
	bestWith, s1, s2 = findOptimalAlignment(cache, strand1, strand2[1:]) if (strand1, strand2[1:]) not in cache else cache[(strand1, strand2[1:])]
	cache[(strand1, strand2[1:])]=(bestWith, s1, s2)
	scores.append((bestWith-2, s1, s2))

	bestWith, s1, s2 = findOptimalAlignment(cache, strand1[1:], strand2) if (strand1[1:], strand2) not in cache else cache[(strand1[1:], strand2)]
	cache[(strand1[1:], strand2)]=(bestWith, s1, s2)
	scores.append((bestWith-2, s1, s2))

	i,v = max(enumerate(scores), key=operator.itemgetter(1))
	#print(i,v,scores)
	if i==0:
		return (v[0], strand1[0]+v[1], strand2[0]+v[2])
	elif i==1:
		return (v[0], ' '+v[1], strand2[0]+v[2])
	else:
		return (v[0], strand1[0]+v[1], ' '+v[2])


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

def printAlignment(score, out = sys.stdout):
	
	resultPlusString,resultMinusString=get_score_strings(score[1],score[2])

	out.write("Optimal alignment score is " + str(score[0]) + "\n\n" +
          "   -  " + resultMinusString + "\n" +
          "      " + score[1] + "\n" +
          "      " + score[2] + "\n" +
          "   +  " + resultPlusString + "\n\n")
	out.write("Optimal alignment score is " + str(score[0]) + "\n")
	#out.write(score[1]+'\n'+score[2]+'\n')

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
	low=8
	high=10
	cache={}
	#zz=40
	while (True):
		sys.stdout.write("Generate random DNA strands? ")
		answer = sys.stdin.readline()
		if answer == "no\n": break
		strand1 = generateRandomDNAStrand(low, high)
		strand2 = generateRandomDNAStrand(low, high)
		sys.stdout.write("\nAligning these two strands: " + strand1 + "\n")
		sys.stdout.write("                            " + strand2 + "\n")
		alignment = findOptimalAlignment(cache,strand1, strand2)
		printAlignment(alignment)
		#test(alignment[0], alignment[1], alignment[2])
	
if __name__ == "__main__":
  main()
