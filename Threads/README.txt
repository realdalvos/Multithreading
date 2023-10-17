Sie muessen pro Sitzung / Job-Lauf einmalig ausfuehren:
   module load gcc libFHBRS

Uebersetzen mit:
    make

Loeschen des ausfuehrbaren Programms:
    make clean

Testen auf Plausibilitaet mit kleinen Vektoren:
    make test

Ausfuehren von Messungen mit unterschiedlichen Threadzahlen auf grossen Vektoren
(  ***** NICHT AUF wr0!!! *****):
    make run

Starten des Job-Skripts:
    sbatch job_vector.sh



Die fuer Sie interessante Methode ist: vectorOperationParallel. Nur dort duerfen Sie Aenderungen vornehmen.
Alle weiteren Randbedingungen ergeben sich auf der Aufgabenstellung.
