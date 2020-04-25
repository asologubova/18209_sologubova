package ru.nsu.ccfit.sologubova.minesweeper;

public class Field {

    int height = 9;
    int width = 9;
    private int numOfMines = 10;
    private int numOfFlags;
    private Cell[][] field;

    public Field(){
        field = new Cell[height][width];
        for(int i = 0; i < height; i++){
            for (int j = 0; j < width; j++){
                field[i][j] = new Cell();
            }
        }
        numOfFlags = 0;
    }

    public Field(int height, int width){
        field = new Cell[height][width];
        for(int i = 0; i < height; i++){
            for (int j = 0; j < width; j++){
                field[i][j] = new Cell();
            }
        }
        this.height = height;
        this.width = width;
        numOfFlags = 0;
    }

    public Field(int height, int width, int mines){
        field = new Cell[height][width];
        for(int i = 0; i < height; i++){
            for (int j = 0; j < width; j++){
                field[i][j] = new Cell();
            }
        }
        this.height = height;
        this.width = width;
        numOfMines = mines;
        numOfFlags = 0;
    }

//    public int getNumOfMines(){
//        return (numOfMines - numOfFlags);
//    }

    public void placeMines(int x, int y){
        for (int k = 0; k < numOfMines; k++){  //расставить мины, избегая квадрат вокруг (x,y)
            boolean ok = false;

            while(!ok){
                int rx = (int) (Math.random() * field.length);
                int ry = (int) (Math.random() * field.length);

                if(!field[rx][ry].isMine()){
                    if (!((rx == x - 1)&&(ry == y - 1)) && !((rx == x)&&(ry == y - 1)) && !((rx == x + 1)&&(ry == y - 1)) &&
                            !((rx == x - 1)&&(ry == y)) && !((rx == x)&&(ry == y)) && !((rx == x + 1)&&(ry == y)) &&
                            !((rx == x - 1)&&(ry == y + 1)) && !((rx == x)&&(ry == y + 1)) && !((rx == x + 1)&&(ry == y + 1))){
                        field[rx][ry].setMine();
                        ok = true;
                    }
                }
            }
        }

        for (int i = 0; i < field.length; i++){  //расставить цифры
            for (int j = 0; j < field.length; j++){
                int counter = 0;
                if(!field[i][j].isMine()){
                    if ((i > 0) && (j > 0) && field[i-1][j-1].isMine() ) counter++;
                    if ((i > 0) && field[i-1][j].isMine()) counter++;
                    if ((i > 0) && (j + 1 < field.length) && field[i-1][j+1].isMine()) counter++;
                    if ((j > 0) && field[i][j-1].isMine()) counter++;
                    if ((j + 1 < field.length) && field[i][j+1].isMine()) counter++;
                    if ((i + 1 < field.length) && (j > 0) && field[i+1][j-1].isMine()) counter++;
                    if ((i + 1 < field.length) && field[i+1][j].isMine()) counter++;
                    if ((i + 1 < field.length) && (j + 1 < field.length) && field[i+1][j+1].isMine()) counter++;
                }
                field[i][j].setNumOfNeighbours(counter);
            }
        }
    }

    public void open(int x, int y){
        field[x][y].open();
    }

    public boolean setFlag(int x, int y){
        if (!field[x][y].isOpened()){
            field[x][y].setflag();
            numOfFlags++;
            return true;
        }
        return false;
    }

    public Cell[][] getFieldInstance(){
        return field;
    }
}
