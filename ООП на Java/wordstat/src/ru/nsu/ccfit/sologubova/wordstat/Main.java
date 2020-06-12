package ru.nsu.ccfit.sologubova.wordstat;

import java.io.*;

public class Main {

    public static void main(String[] args) {

        String fileName = args[0];  //Обработать?

        InputStreamReader reader = null;
        try {
            reader = new InputStreamReader(new FileInputStream(fileName));
            //read the data here:
            MyFileReader wordsReader = new MyFileReader(reader);
        }
        catch (IOException e) {
            System.err.println("Error while reading file: " + e.getLocalizedMessage());
        }
        finally {
            if (null != reader){
                try {
                    reader.close();
                }
                catch (IOException e) {
                    e.printStackTrace(System.err);
                }
            }
        }
    }
}
