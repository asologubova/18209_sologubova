package ru.nsu.ccfit.sologubova.minesweeper;

public class Cell {
    private boolean isOpened;
    private boolean isFlagged;
    private boolean isMine;
    private int numOfNeighbours;

    public Cell(){
        isOpened = false;
        isFlagged = false;
        isMine = false;
        numOfNeighbours = 0;
    }

    public boolean isOpened(){
        return isOpened;
    }

    public boolean isFlagged(){
        return isFlagged;
    }

    public boolean isMine(){
        return isMine;
    }

    public int getNumOfNeighbours(){
        return numOfNeighbours;
    }

    public void open(){
        isOpened = true;
    }

    public void setflag(){
        isFlagged = true;
    }

    public void setMine(){
        isMine = true;
    }

    public void setNumOfNeighbours(int num){
        numOfNeighbours = num;
    }
}
