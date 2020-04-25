package ru.nsu.ccfit.sologubova.minesweeper.gui;

import javax.swing.*;
import java.util.TimerTask;
import java.util.Timer;

public class TimerLabel extends JLabel
{
    private Timer timer = new Timer();
    private TimerTask timerTask;

    public TimerLabel () {
        restartTimer();
    }

    public void restartTimer() {
        stopTimer();
        timerTask = new TimerTask() {
            private volatile int time = -1;
            @Override
            public void run() {
                time++;
                SwingUtilities.invokeLater(new Runnable() {
                    @Override
                    public void run(){
                        int t = time;
                        TimerLabel.this.setText(String.format("%02d:%02d", t / 60, t % 60));
                    }
                });
            }
        };
        timer.schedule(timerTask, 0, 1000);
    }

    public void stopTimer() {
        if (timerTask != null)
            timerTask.cancel();
    }
}