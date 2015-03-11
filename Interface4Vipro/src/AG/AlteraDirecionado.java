/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * AlteraDirecionado.java
 *
 * Created on Jun 1, 2010, 11:52:16 AM
 */

package AG;

import java.awt.Dimension;
import java.util.Vector;
import javax.swing.DefaultComboBoxModel;
import javax.swing.event.ListDataListener;

/**
 *
 * @author pardal
 */
public class AlteraDirecionado extends javax.swing.JFrame {

    public String metodo;
    public String comando;
    public AGconfig pai;

    /** Creates new form AlteraDirecionado */
    public AlteraDirecionado() {
        initComponents();
    }

    public AlteraDirecionado(String alterar, AGconfig pai) {

        initComponents();

        Dimension screenSize = this.getToolkit().getScreenSize();
        this.setLocation((screenSize.width/2) - (this.getSize().width)/2, (screenSize.height/2) - (this.getSize().height)/2);

        this.pai = pai;

        this.setTitle("Alterar " + alterar.split(":")[0]);
        metodo = alterar.split(":")[1].trim();
        comando = alterar;

        Vector vet = new Vector();
        
        if (comando.split(":")[0].equals("Método de cruzamento")){

            vet.add("Ponto de Cruzamento Unico");
            vet.add("Dois Pontos de Cruzamento");
            vet.add("Cruzamento Uniforme");
            vet.add("Cruzamento Aritmético");

        }else if (comando.split(":")[0].equals("Método de mutação")){

            vet.add("Mutação Aleatória");
            vet.add("Inversão de Bits");

        }else if (comando.split(":")[0].equals("Método de seleção")){

            vet.add("Método do Torneio");
            vet.add("Seleção por Ranking");
            vet.add("Seleção Truncada");

        }

        DefaultComboBoxModel model = new DefaultComboBoxModel(vet);
        lista.setModel(model);

        comando = alterar;

    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jButton1 = new javax.swing.JButton();
        jButton2 = new javax.swing.JButton();
        jLabel1 = new javax.swing.JLabel();
        lista = new javax.swing.JComboBox();

        setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);

        jButton1.setText("Ok");
        jButton1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton1ActionPerformed(evt);
            }
        });

        jButton2.setText("Cancelar");

        jLabel1.setText("Text:");

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jButton1, javax.swing.GroupLayout.PREFERRED_SIZE, 245, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jLabel1))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addComponent(jButton2, javax.swing.GroupLayout.DEFAULT_SIZE, 245, Short.MAX_VALUE)
                    .addComponent(lista, 0, 245, Short.MAX_VALUE))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                .addContainerGap(18, Short.MAX_VALUE)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(lista, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jLabel1))
                .addGap(14, 14, 14)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jButton1)
                    .addComponent(jButton2))
                .addContainerGap())
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void jButton1ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton1ActionPerformed

        metodo = (String) lista.getModel().getSelectedItem();

        if (comando.split(":")[0].equals("Método de cruzamento")) pai.setmCruz(metodo);
        else if (comando.split(":")[0].equals("Método de mutação")) pai.setmMut(metodo);
        else if (comando.split(":")[0].equals("Método de seleção")) pai.setmSelec(metodo);

        this.setVisible(false);

    }//GEN-LAST:event_jButton1ActionPerformed

    /**
    * @param args the command line arguments
    */
    /*public static void main(String args[]) {
        java.awt.EventQueue.invokeLater(new Runnable() {
            public void run() {
                new AlteraDirecionado().setVisible(true);
            }
        });
    }*/

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton jButton1;
    private javax.swing.JButton jButton2;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JComboBox lista;
    // End of variables declaration//GEN-END:variables

}
