/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package visao;

import controlador.Controlador;
import java.awt.Color;
import modelo.Ambiente;

/**
 *
 * @author vipro
 */
public class Variarconfig extends javax.swing.JFrame {

    /**
     * Creates new form Variarconfig
     */
    public Variarconfig() {
        initComponents();
    }

    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jLabel5 = new javax.swing.JLabel();
        jTextField9 = new javax.swing.JTextField();
        jTextField10 = new javax.swing.JTextField();
        jLabel1 = new javax.swing.JLabel();
        minMem = new javax.swing.JTextField();
        maxMem = new javax.swing.JTextField();
        maxFun = new javax.swing.JTextField();
        minFun = new javax.swing.JTextField();
        jLabel2 = new javax.swing.JLabel();
        jLabel3 = new javax.swing.JLabel();
        minIL1 = new javax.swing.JTextField();
        maxIL1 = new javax.swing.JTextField();
        jLabel4 = new javax.swing.JLabel();
        minDL1 = new javax.swing.JTextField();
        maxDL1 = new javax.swing.JTextField();
        jLabel6 = new javax.swing.JLabel();
        minScalar = new javax.swing.JTextField();
        maxScalar = new javax.swing.JTextField();
        jLabel7 = new javax.swing.JLabel();
        jLabel8 = new javax.swing.JLabel();
        jButton1 = new javax.swing.JButton();
        jButton2 = new javax.swing.JButton();

        jLabel5.setText("IL1 cache");

        jTextField9.setText("jTextField1");

        jTextField10.setText("jTextField2");

        jLabel1.setText("Memory latency");

        minMem.setText("8");
        minMem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                minMemActionPerformed(evt);
            }
        });

        maxMem.setText("16");
        maxMem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                maxMemActionPerformed(evt);
            }
        });

        maxFun.setText("4");

        minFun.setText("1");

        jLabel2.setText("Functional units");

        jLabel3.setText("IL1 cache");

        minIL1.setText("256");

        maxIL1.setText("65536");
        maxIL1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                maxIL1ActionPerformed(evt);
            }
        });

        jLabel4.setText("IL1 cache");

        minDL1.setText("256");
        minDL1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                minDL1ActionPerformed(evt);
            }
        });

        maxDL1.setText("65536");

        jLabel6.setText("Scalarity ");

        minScalar.setText("1");

        maxScalar.setText("4");

        jLabel7.setText("Min");

        jLabel8.setText("Max");

        jButton1.setText("Variar");
        jButton1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton1ActionPerformed(evt);
            }
        });

        jButton2.setText("Close");
        jButton2.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton2ActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                .addGap(29, 29, 29)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                    .addGroup(layout.createSequentialGroup()
                        .addComponent(jLabel1)
                        .addGap(18, 18, 18)
                        .addComponent(minMem, javax.swing.GroupLayout.PREFERRED_SIZE, 71, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(layout.createSequentialGroup()
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(jLabel2)
                            .addComponent(jLabel3)
                            .addComponent(jLabel4)
                            .addComponent(jLabel6))
                        .addGap(18, 18, 18)
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(minScalar)
                            .addComponent(minDL1)
                            .addComponent(minFun)
                            .addComponent(minIL1))))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                    .addComponent(maxIL1)
                    .addComponent(maxMem)
                    .addComponent(maxFun)
                    .addComponent(maxScalar)
                    .addComponent(maxDL1, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 73, Short.MAX_VALUE))
                .addGap(63, 63, 63))
            .addGroup(layout.createSequentialGroup()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(layout.createSequentialGroup()
                        .addGap(173, 173, 173)
                        .addComponent(jLabel7)
                        .addGap(94, 94, 94)
                        .addComponent(jLabel8))
                    .addGroup(layout.createSequentialGroup()
                        .addGap(103, 103, 103)
                        .addComponent(jButton1)
                        .addGap(137, 137, 137)
                        .addComponent(jButton2)))
                .addContainerGap(85, Short.MAX_VALUE))
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabel7)
                    .addComponent(jLabel8))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabel1)
                    .addComponent(minMem, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(maxMem, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(18, 18, 18)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabel2)
                    .addComponent(minFun, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(maxFun, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(18, 18, 18)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabel3)
                    .addComponent(maxIL1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(minIL1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(18, 18, 18)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabel4)
                    .addComponent(maxDL1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(minDL1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabel6)
                    .addComponent(maxScalar, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(minScalar, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 23, Short.MAX_VALUE)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jButton1)
                    .addComponent(jButton2))
                .addContainerGap())
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void minMemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_minMemActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_minMemActionPerformed

    private void maxMemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_maxMemActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_maxMemActionPerformed

    private void maxIL1ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_maxIL1ActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_maxIL1ActionPerformed

    private void minDL1ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_minDL1ActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_minDL1ActionPerformed

    private void jButton1ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton1ActionPerformed
        // TODO add your handling code here:
        //Warnings("Aguarde. Simulação em Andamento!", Color.red);
        //charge the Ambiente class with the configuration
        Ambiente.minmaxvariar[0][0]= Integer.parseInt(minMem.getText());
        Ambiente.minmaxvariar[0][1]= Integer.parseInt(maxMem.getText());
        Ambiente.minmaxvariar[1][0]= Integer.parseInt(minFun.getText());
        Ambiente.minmaxvariar[1][1]= Integer.parseInt(maxFun.getText());
        Ambiente.minmaxvariar[2][0]= Integer.parseInt(minIL1.getText());
        Ambiente.minmaxvariar[2][1]= Integer.parseInt(maxIL1.getText());
        Ambiente.minmaxvariar[3][0]= Integer.parseInt(minDL1.getText());
        Ambiente.minmaxvariar[3][1]= Integer.parseInt(maxDL1.getText());
        Ambiente.minmaxvariar[4][0]= Integer.parseInt(minScalar.getText());
        Ambiente.minmaxvariar[4][1]= Integer.parseInt(maxScalar.getText());
        
        Controlador.controlador("variar");
        //Warnings("Simulação Concluida com Sucesso!", Color.BLUE);

    }//GEN-LAST:event_jButton1ActionPerformed

    private void jButton2ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton2ActionPerformed
        // TODO add your handling code here:
        this.setVisible(false);
    }//GEN-LAST:event_jButton2ActionPerformed

  
   

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton jButton1;
    private javax.swing.JButton jButton2;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JLabel jLabel3;
    private javax.swing.JLabel jLabel4;
    private javax.swing.JLabel jLabel5;
    private javax.swing.JLabel jLabel6;
    private javax.swing.JLabel jLabel7;
    private javax.swing.JLabel jLabel8;
    private javax.swing.JTextField jTextField10;
    private javax.swing.JTextField jTextField9;
    private javax.swing.JTextField maxDL1;
    private javax.swing.JTextField maxFun;
    private javax.swing.JTextField maxIL1;
    private javax.swing.JTextField maxMem;
    private javax.swing.JTextField maxScalar;
    private javax.swing.JTextField minDL1;
    private javax.swing.JTextField minFun;
    private javax.swing.JTextField minIL1;
    private javax.swing.JTextField minMem;
    private javax.swing.JTextField minScalar;
    // End of variables declaration//GEN-END:variables
}
