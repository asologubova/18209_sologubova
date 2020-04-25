package ru.nsu.ccfit.sologubova.minesweeper.gui;

import ru.nsu.ccfit.sologubova.minesweeper.Cell;
import ru.nsu.ccfit.sologubova.minesweeper.Field;

import javax.swing.*;
import java.awt.*;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

public class GameFieldPanel extends JPanel {
    Field field;  //размеры и мины указать
    private int width = 9;
    private int height = 9;
    private int cellSize = 40;
    JButton buttonList[][];
    boolean flag = false;

    GameFieldPanel(){
        super();
        this.setLayout(new GridLayout(height, width));
        buttonList = new JButton[height][width];
        field = new Field(height, width);  //размеры и мины указать
        fillField();
    }

    GameFieldPanel(int height, int width){
        super();
        this.setLayout(new GridLayout(height, width));
        this.height = height;
        this.width = width;
        buttonList = new JButton[height][width];
        fillField();
    }

    private void fillField(){
        Dimension d = new Dimension(cellSize, cellSize);
        for(int i = 0; i < height; i++){
            for (int j = 0; j < width; j++){
                JButton button = new JButton();
                button.setPreferredSize(d);
                button.addMouseListener(new CellEventListener());
                buttonList[i][j] = button;
                this.add(buttonList[i][j]);
            }
        }

        //setEnabled(boolean enabled)
        //setVisible(boolean aFlag)
    }

    public Dimension getMinimumSize(){
        Dimension size = new Dimension();
        size.height += cellSize * height;
        size.width += cellSize * width;
        return size;
    }

    class CellEventListener implements MouseListener {  //от интерфейса ActionLstener (обработчик событий)

        @Override
        public void mouseClicked(MouseEvent mouseEvent) {  //сделать ход
            if(mouseEvent.getClickCount() == 2){
                //поставить флаг
                return;
            }

            int x = 0, y = 0;
            //Посчитать координаты!!!
            if (flag == false){
                field.placeMines(x,y);
                flag = true;
            }
            field.open(x,y);
            Cell[][] fieldInstance = field.getFieldInstance();
            if (fieldInstance[x][y].isMine()){
                //endOfGame();
            }
            else if (fieldInstance[x][y].isOpened()){// && (fieldInstance[x][y].getNumOfNeighbours() > 0)){
                int n = fieldInstance[x][y].getNumOfNeighbours();
                buttonList[x][y].setText(String.valueOf(n));
                //buttonList[x][y].setText("1");
            }
            //[x][y].setText("1");

        }

        @Override
        public void mousePressed(MouseEvent mouseEvent) {}

        @Override
        public void mouseReleased(MouseEvent mouseEvent) {}

        @Override
        public void mouseEntered(MouseEvent mouseEvent) {}

        @Override
        public void mouseExited(MouseEvent mouseEvent) {}
    }

}
