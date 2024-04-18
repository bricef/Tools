
import string
import re

alphabet = string.ascii_lowercase

with open("spellnames.txt") as f:
  spells = [w.strip() for w in f.readlines()]

with open("words.txt") as f:
  valid_words = set([w.strip() for w in f.readlines()])

def tokenise(spellname):
  return re.split('(\W+)', spellname)
  
def distance1(word):
  possible = []
  if len(word) <= 1:
    return [word]
  if word == "of":
    return ["of"]
  if word == "ua":
    return ["ua"]

  for c in alphabet :
    possible.append(c+word)
  for i, cw in enumerate(word):
    possible.append(''.join(word[:i]+word[i+1:]))
    for c in [c for c in alphabet if c != cw] :
      possible.append(''.join(word[:i]+c+word[i+1:]))
  for c in alphabet :
    possible.append(word+c)
  return [p for p in possible if is_valid(p)]

def is_valid(word):
  return word in valid_words

def spellwords(spells):
  spellwords = set()
  for spell in spells:
    tokens = tokenise(spell)
    spellwords.update(*tokens)
  return spellwords

substitutions = {}
for word in spellwords(spells):
  substitutions[word] = distance1(word)

def possible_spells(spell):
  possibles = []
  tokens = tokenise(spell)
  for i, token in enumerate(tokens):
    for sub in distance1(token):
      possibles.append(''.join(tokens[:i]+[sub]+tokens[i+1:]))
  return set(possibles)
      

for spell in spells:
  print(spell.title())
  for possible in possible_spells(spell):
    print("    "+possible)

# print(possible_spells("kill word"))

