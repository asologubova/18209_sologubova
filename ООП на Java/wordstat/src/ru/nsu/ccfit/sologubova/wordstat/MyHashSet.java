package ru.nsu.ccfit.sologubova.wordstat;

import java.util.*;

public class MyHashSet {
    private int sumFreq = 0;
    private Map<String, Integer> set = new HashMap<String, Integer>();

    public boolean insert(String word) {
        if (word.length() == 0) {
            return false;
        }
        if (set.containsKey(word.toLowerCase())) {
            set.put(word, set.get(word) + 1);
        } else {
            set.put(word, 1);
        }
        sumFreq++;
        return true;
    }

    public void sort(){
        
    }
}
