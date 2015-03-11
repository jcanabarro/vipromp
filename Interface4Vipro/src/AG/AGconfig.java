/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * AGconfig.java
 *
 * Created on May 31, 2010, 10:03:16 AM
 */

package AG;

import java.awt.Dimension;
import javax.swing.JFrame;

/**
 *
 * @author pardal
 */
public class AGconfig extends javax.swing.JFrame {

    public static int popIni;
    public static int gerac;
    public static int elitismo;
    public static int mut;
    public static int cruzamento;

    public static String metodoCruz;
    public static String metodoSelec;
    public static String metodoMut;

    public static int AuxpopIni;
    public static int Auxgerac;
    public static int Auxelitismo;
    public static int Auxmut;
    public static int Auxcruzamento;

    public static String AuxmetodoCruz;
    public static String AuxmetodoSelec;
    public static String AuxmetodoMut;

    public JFrame pai;

    /** Creates new form AGconfig */
    public AGconfig() {

        initComponents();

        this.setTitle("AG config");

        Dimension screenSize = this.getToolkit().getScreenSize();
        this.setLocation((screenSize.width/2) - (this.getSize().width)/2, (screenSize.height/2) - (this.getSize().height)/2);

        AGconfig.AuxpopIni      = AGconfig.popIni;
        AGconfig.Auxgerac       = AGconfig.gerac;
        AGconfig.Auxelitismo    = AGconfig.elitismo;
        AGconfig.Auxmut         = AGconfig.mut;
        AGconfig.Auxcruzamento  = AGconfig.cruzamento;

        AGconfig.AuxmetodoCruz  = AGconfig.metodoCruz;
        AGconfig.AuxmetodoMut   = AGconfig.metodoMut;
        AGconfig.AuxmetodoSelec = AGconfig.metodoSelec;
        
        this.plotar();

    }

    public AGconfig(JFrame pai) {

        initComponents();

        Dimension screenSize = this.getToolkit().getScreenSize();
        this.setLocation((screenSize.width/2) - (this.getSize().width)/2, (screenSize.height/2) - (this.getSize().height)/2);

        this.setTitle("AG config");

        this.pai = pai;
        
        AGconfig.AuxpopIni      = AGconfig.popIni;
        AGconfig.Auxgerac       = AGconfig.gerac;
        AGconfig.Auxelitismo    = AGconfig.elitismo;
        AGconfig.Auxmut         = AGconfig.mut;
        AGconfig.Auxcruzamento  = AGconfig.cruzamento;

        AGconfig.AuxmetodoCruz  = AGconfig.metodoCruz;
        AGconfig.AuxmetodoMut   = AGconfig.metodoMut;
        AGconfig.AuxmetodoSelec = AGconfig.metodoSelec;

        this.plotar();

    }

    public AGconfig(JFrame pai, int popIni, int gerac, int elitismo, int mut, int cruzamento, String metodoCruz, String metodoSelec, String metodoMut) {

        initComponents();

        Dimension screenSize = this.getToolkit().getScreenSize();
        this.setLocation((screenSize.width/2) - (this.getSize().width)/2, (screenSize.height/2) - (this.getSize().height)/2);

        this.setTitle("AG config");

        this.pai = pai;

        AGconfig.popIni = popIni;
        AGconfig.gerac = gerac;
        AGconfig.elitismo = elitismo;
        AGconfig.mut = mut;
        AGconfig.cruzamento = cruzamento;

        AGconfig.metodoCruz = metodoCruz;
        AGconfig.metodoMut = metodoMut;
        AGconfig.metodoSelec = metodoSelec;

        this.plotar();

    }

    public static void Inic(){

        popIni      = 10;
        gerac       = 300;
        elitismo    = 20;
        mut         = 75;
        cruzamento  = 80;

        metodoCruz  = "Ponto de Cruzamento Unico";
        metodoMut   = "Mutação Aleatória";
        metodoSelec = "Método do Torneio";

        AGconfig.AuxpopIni      = AGconfig.popIni;
        AGconfig.Auxgerac       = AGconfig.gerac;
        AGconfig.Auxelitismo    = AGconfig.elitismo;
        AGconfig.Auxmut         = AGconfig.mut;
        AGconfig.Auxcruzamento  = AGconfig.cruzamento;

        AGconfig.AuxmetodoCruz  = AGconfig.metodoCruz;
        AGconfig.AuxmetodoMut   = AGconfig.metodoMut;
        AGconfig.AuxmetodoSelec = AGconfig.metodoSelec;

    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        popIniL = new javax.swing.JLabel();
        jButton1 = new javax.swing.JButton();
        jButton2 = new javax.swing.JButton();
        geracL = new javax.swing.JLabel();
        jButton3 = new javax.swing.JButton();
        elitismoL = new javax.swing.JLabel();
        jButton4 = new javax.swing.JButton();
        jButton5 = new javax.swing.JButton();
        jButton6 = new javax.swing.JButton();
        jButton7 = new javax.swing.JButton();
        jButton8 = new javax.swing.JButton();
        mutL = new javax.swing.JLabel();
        cruzL = new javax.swing.JLabel();
        mCruzL = new javax.swing.JLabel();
        mSelecL = new javax.swing.JLabel();
        mMutL = new javax.swing.JLabel();
        jButton9 = new javax.swing.JButton();
        jButton10 = new javax.swing.JButton();

        setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);

        popIniL.setText("População inicial:");

        jButton1.setText("Alterar");
        jButton1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton1ActionPerformed(evt);
            }
        });

        jButton2.setText("Alterar");
        jButton2.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton2ActionPerformed(evt);
            }
        });

        geracL.setText("Número máx. de gerações:");

        jButton3.setText("Alterar");
        jButton3.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton3ActionPerformed(evt);
            }
        });

        elitismoL.setText("Taxa de elitismo:");

        jButton4.setText("Alterar");
        jButton4.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton4ActionPerformed(evt);
            }
        });

        jButton5.setText("Alterar");
        jButton5.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton5ActionPerformed(evt);
            }
        });

        jButton6.setText("Alterar");
        jButton6.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton6ActionPerformed(evt);
            }
        });

        jButton7.setText("Alterar");
        jButton7.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton7ActionPerformed(evt);
            }
        });

        jButton8.setText("Alterar");
        jButton8.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton8ActionPerformed(evt);
            }
        });

        mutL.setText("Taxa de mutação:");

        cruzL.setText("Taxa de cruzamento:");

        mCruzL.setText("Método de cruzamento:");

        mSelecL.setText("Método de seleção:");

        mMutL.setText("Método de mutação:");

        jButton9.setText("OK");
        jButton9.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton9ActionPerformed(evt);
            }
        });

        jButton10.setText("Cancelar");
        jButton10.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton10ActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(mutL)
                            .addComponent(cruzL)
                            .addComponent(elitismoL)
                            .addComponent(mCruzL)
                            .addComponent(geracL)
                            .addComponent(popIniL)
                            .addComponent(mMutL)
                            .addComponent(mSelecL))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 263, Short.MAX_VALUE)
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                            .addComponent(jButton3, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                            .addComponent(jButton2, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                            .addComponent(jButton1, javax.swing.GroupLayout.DEFAULT_SIZE, 110, Short.MAX_VALUE)
                            .addComponent(jButton4, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                            .addComponent(jButton5, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                            .addComponent(jButton6, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                            .addComponent(jButton7, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                            .addComponent(jButton8, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)))
                    .addGroup(layout.createSequentialGroup()
                        .addComponent(jButton9, javax.swing.GroupLayout.PREFERRED_SIZE, 250, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                        .addComponent(jButton10, javax.swing.GroupLayout.PREFERRED_SIZE, 250, javax.swing.GroupLayout.PREFERRED_SIZE)))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jButton1)
                    .addComponent(popIniL, javax.swing.GroupLayout.PREFERRED_SIZE, 13, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jButton2)
                    .addComponent(geracL))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jButton3)
                    .addComponent(elitismoL))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jButton4)
                    .addComponent(mutL))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jButton5)
                    .addComponent(cruzL))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jButton6)
                    .addComponent(mCruzL))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jButton7)
                    .addComponent(mSelecL))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jButton8)
                    .addComponent(mMutL))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jButton9)
                    .addComponent(jButton10))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void jButton1ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton1ActionPerformed
        
        Alterar altera = new Alterar (popIniL.getText(), this);
        altera.setVisible(true);

    }//GEN-LAST:event_jButton1ActionPerformed

    private void jButton5ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton5ActionPerformed

        Alterar altera = new Alterar (cruzL.getText(), this);
        altera.setVisible(true);

    }//GEN-LAST:event_jButton5ActionPerformed

    private void jButton2ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton2ActionPerformed

        Alterar altera = new Alterar (geracL.getText(), this);
        altera.setVisible(true);

    }//GEN-LAST:event_jButton2ActionPerformed

    private void jButton3ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton3ActionPerformed

        Alterar altera = new Alterar (elitismoL.getText(), this);
        altera.setVisible(true);

    }//GEN-LAST:event_jButton3ActionPerformed

    private void jButton4ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton4ActionPerformed

        Alterar altera = new Alterar (mutL.getText(), this);
        altera.setVisible(true);

    }//GEN-LAST:event_jButton4ActionPerformed

    private void jButton6ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton6ActionPerformed


        AlteraDirecionado altera = new AlteraDirecionado (mCruzL.getText(), this);
        altera.setVisible(true);

    }//GEN-LAST:event_jButton6ActionPerformed

    private void jButton7ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton7ActionPerformed

        AlteraDirecionado altera = new AlteraDirecionado (mSelecL.getText(), this);
        altera.setVisible(true);

    }//GEN-LAST:event_jButton7ActionPerformed

    private void jButton8ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton8ActionPerformed

        AlteraDirecionado altera = new AlteraDirecionado (mMutL.getText(), this);
        altera.setVisible(true);

    }//GEN-LAST:event_jButton8ActionPerformed

    private void jButton10ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton10ActionPerformed

        AGconfig.popIni      = AGconfig.AuxpopIni;
        AGconfig.gerac       = AGconfig.Auxgerac;
        AGconfig.elitismo    = AGconfig.Auxelitismo;
        AGconfig.mut         = AGconfig.Auxmut;
        AGconfig.cruzamento  = AGconfig.Auxcruzamento;

        AGconfig.metodoCruz  = AGconfig.AuxmetodoCruz;
        AGconfig.metodoMut   = AGconfig.AuxmetodoMut;
        AGconfig.metodoSelec = AGconfig.AuxmetodoSelec;

        this.setVisible(false);
        
    }//GEN-LAST:event_jButton10ActionPerformed

    private void jButton9ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton9ActionPerformed

        this.setVisible(false);

    }//GEN-LAST:event_jButton9ActionPerformed

    /**
    * @param args the command line arguments
    */
    /*public static void main(String args[]) {
        java.awt.EventQueue.invokeLater(new Runnable() {
            public void run() {
                new AGconfig().setVisible(true);
            }
        });
    }*/

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JLabel cruzL;
    private javax.swing.JLabel elitismoL;
    private javax.swing.JLabel geracL;
    private javax.swing.JButton jButton1;
    private javax.swing.JButton jButton10;
    private javax.swing.JButton jButton2;
    private javax.swing.JButton jButton3;
    private javax.swing.JButton jButton4;
    private javax.swing.JButton jButton5;
    private javax.swing.JButton jButton6;
    private javax.swing.JButton jButton7;
    private javax.swing.JButton jButton8;
    private javax.swing.JButton jButton9;
    private javax.swing.JLabel mCruzL;
    private javax.swing.JLabel mMutL;
    private javax.swing.JLabel mSelecL;
    private javax.swing.JLabel mutL;
    private javax.swing.JLabel popIniL;
    // End of variables declaration//GEN-END:variables

    public void plotar(){

        popIniL.setText("População inicial: " + AGconfig.popIni);
        geracL.setText("Número máx. de gerações: " + AGconfig.gerac);
        elitismoL.setText("Taxa de elitismo: " + AGconfig.elitismo);
        mutL.setText("Taxa de mutação: " + AGconfig.mut);
        cruzL.setText("Taxa de cruzamento: " + AGconfig.cruzamento);

        mCruzL.setText("Método de cruzamento: " + AGconfig.metodoCruz);
        mMutL.setText("Método de mutação: " + AGconfig.metodoMut);
        mSelecL.setText("Método de seleção: " + AGconfig.metodoSelec);

    }

    public String removeSpaces(String aux){

        String[] aux2 = aux.split(" ");

        aux = aux2[0];
        for (int i = 1; i < aux2.length; i++) aux += ":&:" + aux2[i];

        return aux;

    }

    public void setPopIni(int valor){
        
        popIni = valor;
        plotar();

    }

    public void setGerac(int valor){

        AGconfig.gerac = valor;
        plotar();

    }

    public void setElitismo(int valor){

        AGconfig.elitismo = valor;
        plotar();

    }

    public void setMut(int valor){

        AGconfig.mut = valor;
        plotar();

    }

    public void setCruz(int valor){

        AGconfig.cruzamento = valor;
        plotar();

    }

    public void setmCruz(String metodo){

        AGconfig.metodoCruz = metodo;
        plotar();

    }

    public void setmSelec(String metodo){

        AGconfig.metodoSelec = metodo;
        plotar();

    }

    public void setmMut(String metodo){

        AGconfig.metodoMut = metodo;
        plotar();

    }

    public static int getPopIni(){

        return popIni;

    }

    public static int getGerac(){

        return gerac;

    }

    public static int getTaxElit(){

        int retorno = (int) ((double) elitismo / 100 * (double)popIni);
        return retorno;

    }

    public static String getSelec(){

        return metodoSelec;

    }

    public static int getCruz(){

        return cruzamento;

    }

    public static int getMut(){

        return mut;

    }

}
