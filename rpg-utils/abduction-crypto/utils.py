import json
import os.path
import math
import sys
import random
import string
from collections import defaultdict

def stripNotIn(charset, orig):
    return [c for c in orig if c in charset]

def alphanumOnly(s):
    return stripNotIn(set(string.letters+string.digits), s)

def alphaOnly(s):
    return stripNotIn(set(string.letters), s)

def frequencyCount(txt, ngram=1):
    frequencies=defaultdict(int)
    for segment in splitByN(txt, ngram):
        frequencies[segment] += 1
    return frequencies

def splitByN(seq, n):
	return [seq[i:i+n] for i in range(0, len(seq), n)]

def blockFormat(text, blockwidth=5, blocks=8):
	text=text.upper()
	letters = set(string.ascii_uppercase)
	return "\n".join(map(lambda x: " ".join(x), splitByN(splitByN(text, blockwidth), blocks)))

def nGramFrequencies(n, seq):
    freqs = defaultdict(int)
    nGrams = len(seq)-n
    for i in range(nGrams):
        substring = tuple(seq[i:i+n]) if n > 1 else seq[i:i+n][0]
        freqs[substring] += 1
    for k in freqs.keys():
        freqs[k] = freqs[k] / float(nGrams+1)
    return dict(freqs)

def get_language_reference(saved_stats_filepath, corpus_filepath):
    if os.path.isfile(saved_stats_filepath):
        with open(saved_stats_filepath, 'r') as fp:
            reference = json.load(fp)
    else:
        text =  getText(corpus_filepath)
        reference = {
            "oneGrams": nGramFrequencies(1,text),
            "twoGrams": nGramFrequencies(2,text),
            "threeGrams": nGramFrequencies(3,text)
        }
        with open(filepath, 'w') as fp:
            json.dump(reference, fp)
    return reference

def read_lines(filepath):
    with open(filepath) as f:
        lines = [l.strip() for l in f.readlines()]
    return lines

def read_text(fname):
    text = ""
    with open(fname, 'r') as fp:
        text = fp.read()
    return text

def compareHistograms(hist1,hist2):
    # Kullback Leibler Divergence (discrete)
    frequencies = [(hist1[k], hist2[k]) for k in hist1.keys() if k in hist2]
    return sum([pow(abs(t-r),2) for r,t in frequencies])

def matchesReference(text, reference):
	"""Will return how well a text matches the reference language,
	where 'reference' encodes the language characteristics.

	a return value of 0.0 is a perfect match, with greater
	values signifying worse matches"""
	oneGrams = nGramFrequencies(1,text)
	twoGrams = nGramFrequencies(2,text)
	# threeGrams = nGramFrequencies(3,text)
	return compareHistograms(reference["twoGrams"], twoGrams)

def best_guess(seq, reference):
	seq_onegrams = nGramFrequencies(1,seq)
	sorted_seq_onegrams = sorted(seq_onegrams.items(), key=lambda x: x[1])
	sorted_ref_onegrams = sorted(reference["oneGrams"].items(), key=lambda x: x[1])
	maxlen = min([len(sorted_ref_onegrams), len(sorted_seq_onegrams)])
	sorted_seq_onegrams = sorted_seq_onegrams[-maxlen:]
	sorted_ref_onegrams = sorted_ref_onegrams[-maxlen:]
	mapping = dict([(t[0],r[0]) for t,r in zip(sorted_seq_onegrams, sorted_ref_onegrams)])
	return mapping

def process(text):
    return "".join([c for c in text if c in string.ascii_uppercase])

def mapping2decrypt(mapping):
    def decrypt(seq):
        ar = [mapping[symbol] if symbol in mapping else "_" for symbol in seq]
        return "".join(ar)
    return decrypt

def orderedStringFromOneGramFrequencies(char_freq_dict):
	sorted_chars_freqs = sorted(char_freq_dict.items(), key=lambda x: x[1])
	decreasing = sorted_chars_freqs[::-1]
	just_chars = [c for c,v in decreasing]
	return "".join( just_chars )


#
# Genetic search solver
#

def decipherWith(individual, ciphertext):
	decryption_function = mapping2decrypt(individual.genome)
	plaintex_candidate = decryption_function(ciphertext)
	return plaintex_candidate

class Individual(object):
	ciphertext = None
	language_ref = None

	@classmethod
	def fromString(cls, keyStr):
		genome = dict(zip(string.uppercase, keyStr))
		return Individual(genome)

	def __init__(self, genome):
		self.genome = genome
		self._fitness = None

	def fitness(self):
		if self._fitness:
			return self._fitness
		decryptionFunction = mapping2decrypt(self.genome)
		attempted_decryption = decryptionFunction(Individual.ciphertext)
		self._fitness = 1.0/matchesReference(attempted_decryption, Individual.language_ref)
		if self._fitness < 0.0:
			self._fitness = 0.0
		assert self._fitness >= 0.0
		return self._fitness

	def crossover(self, other):
		ks1 = getKeyString(self.genome)
		ks2 = getKeyString(other.genome)
		# print self
		# print other
		# assert len(ks1) == 26
		# assert len(ks2) == 26
		alpahabetByFrequency = orderedStringFromOneGramFrequencies(self.language_ref["oneGrams"])
		charsByIndex = [(c, (ks1.index(c) + ks2.index(c))/2.0 ) for c in alpahabetByFrequency]
		orderedCharsByIndex = sorted(charsByIndex, key=lambda x: x[1])
		orderedChars = map(lambda x: x[0], orderedCharsByIndex)
		genome = dict(zip(string.uppercase, orderedChars))
		return Individual(genome)

	def __repr__(self):
		return str(self)

	def __str__(self):
		return "Individual({}, {})".format(getKeyString(self.genome), self.fitness())



def GA(population, mutate, select, stopPredicate, iterationCallback=None, maxiter=10000):
	pop = population
	iters = 0
	while iters < maxiter and not stopPredicate(pop):
		iters += 1
		breedingPairs, survivors = select(pop)
		children = [ mutate(a.crossover(b)) for a,b in breedingPairs ]
		pop = children + survivors
		if iterationCallback:
			iterationCallback(pop, iters)
	best = max(pop, key=lambda x: x.fitness())
	return best

def mutate(individual):
	mutations = 5
	def swap(individual):
		k1 = random.choice(individual.genome.keys())
		k2 = random.choice(individual.genome.keys())
		v1 = individual.genome[k1]
		individual.genome[k1] = individual.genome[k2]
		individual.genome[k2] = v1
		return individual
	for i in range(mutations):
		individual = swap(individual)
	return individual

def getKeyString(mapping):
	kvPairs = sorted(mapping.items(), key=lambda x:x[0])
	return "".join([v for k,v in kvPairs])


def select(population, preservation_rate=0.5):
	# fitness proportional selection
	num_survivors = int(preservation_rate*len(population))

	def select_N(N, population):
		# print "start select pop"
		total_fitness = sum([individual.fitness() for individual in population])
		selected = set([])
		population = set(population)
		while len(selected) < N:
			# print "selecting new individual"
			selector = random.random()*total_fitness
			assert selector >= 0.0
			assert total_fitness >= 0.0
			acc = 0.0
			for i in population:
				# print "Accu: {}, Selector: {} Total Fitness: {} I's Fitness: {}".format(acc, selector, total_fitness, i.fitness())
				if (acc < selector <= (acc + i.fitness())):
					total_fitness -= i.fitness()
					selected.add(i)
					population.remove(i)
					break
				acc += i.fitness()
			# print "done selecting individual"
		# print "endselect pop"
		return list(selected)

	survivors = sorted(population, key=lambda i: i.fitness())[-num_survivors:]

	# survivors = select_N(num_survivors, population)
	num_breeding_pairs = len(population) - num_survivors
	breedingPairs = [(random.choice(survivors), random.choice(population)) for i in range(num_breeding_pairs)]
	return (breedingPairs, survivors)

def generateIndividual():
	return Individual.fromString(sorted(string.uppercase, key=lambda x: random.random()))