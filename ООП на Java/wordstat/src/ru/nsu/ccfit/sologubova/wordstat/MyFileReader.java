package ru.nsu.ccfit.sologubova.wordstat;

import java.io.IOException;
import java.io.InputStreamReader;

public class MyFileReader {

    private InputStreamReader isr;

    MyFileReader(InputStreamReader isr){
        this.isr = isr;
    }

    public String getWord() throws IOException {
        int c;
        char symbol;
        StringBuilder word = new StringBuilder();

        while((c = isr.read()) != -1){
            symbol = (char) c;

            if(Character.isLetterOrDigit(symbol)){
                word.append(Character.toLowerCase(symbol));
            }
            else{
                if(word.length() != 0){
                    return word.toString();
                }
                else{
                    continue;
                }
            }
        }

        return word.toString();
    }
}
