package ru.nsu.ccfit.sologubova.minesweeper.gui;

import java.awt.*;
import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

//количество мин, кнопка рестарта, режим (флаг/открытие ячеек), время, таблица с полем

public class MinesweeperLayout extends JFrame implements LayoutManager{ //окно

    private Dimension size = new Dimension();
    private JLabel minesLabel = new JLabel("10");  //обновление после каждого поставленного флажка
    private TimerLabel timeLabel = new TimerLabel();  //время, затраченное на игру (потоки)
    private JButton settingsButton = new JButton("Settings");
    private JButton newGameButton = new JButton("New Game");
    private JButton gameModeButton = new JButton("Game Mode");
    private GameFieldPanel gameField;  //GridLayout
    //расположение элементов - GridBagLayout

    public MinesweeperLayout(){
        super("Minesweeper");  // создаем окно
        this.setBounds(100,100,400,400);  // размеры
        this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        Container container = this.getContentPane();
        container.setLayout(new GridBagLayout());

        gameField = new GameFieldPanel();

        newGameButton.addActionListener(new ButtonEventListener()); //поможет вызвать класс с реализацией действия

        layoutContainer(container);
        Dimension s = minimumLayoutSize(container);
        //System.out.println(s);
        this.setMinimumSize(s);  //использовать minimumLayoutSize
        this.setVisible(true);  // открываем окно
    }

//не используем:
    @Override
    public void addLayoutComponent(String s, Component component) {}

    @Override
    public void removeLayoutComponent(Component component) {}

//определение минимального размера контейнера:
    @Override
    public Dimension preferredLayoutSize(Container container) {
        return size;
    }

//определение предпочтительного размера контейнера:
    @Override
    public Dimension minimumLayoutSize(Container container) {
        int width = 0;
        int height = 30;

        height += gameField.getMinimumSize().height;
        width += gameField.getMinimumSize().width;

        size.height = height + 4 * 5 + 10;
        size.width = width + 4 * 5 + 10;
        return size;
    }

    @Override
    public void layoutContainer(Container container) {
        GridBagConstraints constraints = new GridBagConstraints();
        // По умолчанию натуральная высота, максимальная ширина
        constraints.fill = GridBagConstraints.HORIZONTAL;
        //constraints.weightx = 60;
        constraints.gridy = 0  ;  // нулевая ячейка таблицы по вертикали

        constraints.gridx = 0;  // нулевая ячейка таблицы по горизонтали
        container.add(minesLabel, constraints);
        constraints.gridx = 1;
        container.add(settingsButton, constraints);
        constraints.gridx = 2;
        container.add(newGameButton, constraints);
        constraints.gridx = 3;
        container.add(gameModeButton, constraints);
        constraints.gridx = 4;
        container.add(timeLabel, constraints);

        constraints.gridx = 0;    // нулевая ячейка по горизонтали
        constraints.gridy = 1;    // первая ячейка по вертикали
        constraints.gridwidth = 5;  //сколько ячеек горизонтально занимает
        constraints.gridheight = 5;
        container.add(gameField, constraints);
    }

    class ButtonEventListener implements ActionListener {  //от интерфейса ActionLstener (обработчик событий)
        public void actionPerformed(ActionEvent e){ //только если началась игра (нажата первая кнопка на поле
            timeLabel.restartTimer();
        }
    }
}
