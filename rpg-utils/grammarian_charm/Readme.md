# Grammarian Charm
Once upon a time a DM gave his players an interesting item. 

Once and only once, they would be able to change the name of a spell by by adding a letter, changing a letter or removing a letter (edit distance 1). 

The scripts in this repo get a list of valid D&D spells, then use a wordlist to generate new spells with an edit distance of 1 from the original list.

You can find the list of alternative spells in `alternative_spells.txt` and the script to generate then in `permutations.py`
