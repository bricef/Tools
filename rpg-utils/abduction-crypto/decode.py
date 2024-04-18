#!/usr/bin/env python3

import string

from collections import defaultdict
from pprint import pprint

import utils

def display_frequencies(hist):
    print()
    sorted_frequencies = sorted(hist.items(), key=lambda x: -x[1])
    vmax = sorted_frequencies[0][1]
    for k,v in sorted_frequencies:
        size = int((v/vmax)*100)
        print("{:2} {}".format(k,"="*size))
    print()

def symbol_to_alphabet_map(symbols, alphabet):
    if len(symbols) > len(alphabet):
        raise RuntimeError("More symbols than alphabet can represent")
    return dict(zip(symbols, alphabet))

def symbolic_cipher_to_alphabetic_cipher(cipher_symbolic):
    symbol_map = symbol_to_alphabet_map(sorted(set(cipher_symbolic)), string.ascii_uppercase)
    cipher = "".join(utils.mapping2decrypt(symbol_map)(cipher_symbolic))
    return cipher

if __name__ == "__main__":
    CORPUS_PATH = "resources/corpus_en.txt"
    SAVED_STATS_PATH = "resources/english_stats.json"

    cipher1 = symbolic_cipher_to_alphabetic_cipher(utils.read_lines("ciphertext-1.txt"))
    print("Cipher 1:")
    print("=========")
    print(cipher1)
    print("")

    cipher2 = symbolic_cipher_to_alphabetic_cipher(utils.read_lines("ciphertext-2.txt"))
    print("Cipher 2:")
    print("=========")
    print(cipher2)



    en_reference = utils.get_language_reference(SAVED_STATS_PATH, CORPUS_PATH)
    corpus_text = utils.read_text(CORPUS_PATH)

    best_mapping1 = utils.best_guess(cipher1,en_reference)
    best_mapping2 = utils.best_guess(cipher2,en_reference)

    display_frequencies(utils.nGramFrequencies(1, cipher1))
    display_frequencies(utils.nGramFrequencies(1, cipher2))
