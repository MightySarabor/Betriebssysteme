# Betriebssysteme Abgabe 1

## Aufgabe 1: System-Call 

**Aufgabenstellung:**
Finden Sie den Sourcecode für einen beliebigen System-Call-Wrapper (z.B. `read()` oder `write()`) und lokalisieren Sie die zugehörige Gegenstelle im Sourcecode des Linux-Kernels. Schildern Sie auf dieser Grundlage die wesentlichen Schritte und Abläufe, die mit dem Aufruf eines System-Calls auf Anwendungsebene zusammenhängen.

**Meine Wahl:**
Ich habe mich für den Befehl `getpid()` entschieden. 

**Umgebung:**
Meine Umgebung ist Ubuntu in WSL2.

Vorbereitung:
Bevor ich angefangen habe den Befehl zu suchen, habe ich https://github.com/torvalds/linux geklont, um auf den gesamten Sourcecode zugreifen zu können.

## Schritte zur Analyse des System-Calls `getpid()`

### 1. Definition von `getpid` in `unistd.h`

Den System-Call-Wrapper getpid() habe ich in usr/inclunde/unistd.h gesucht. Dort sind auch viele andere System-Call-Wrapper, wie fork(), exec, read() usw.

**Befehl:**
```bash
(base) root@DESKTOP-OL5SHQL:/usr/src/glibc# grep -rnw 'getpid' /usr/include/unistd.h
```
**Ausgabe:**
```c
650:extern __pid_t getpid (void) __THROW;
```
### Look-Up in der syscalls Tabelle

In der Ausgabe sehen wir die Deklaration von getpid(). Der Weg zum eigentlich System-Call ist in folgender Datei definiert.

**Befehl:**
```bash
(base) root@DESKTOP-OL5SHQL:/usr/include# grep -rnw 'getpid' /usr/src/glibc/linux/arch/x86/entry/syscalls/
```

**Ausgabe:**
```c
/usr/src/glibc/linux/arch/x86/entry/syscalls/syscall_64.tbl:51:39       common  getpid                  sys_getpid
/usr/src/glibc/linux/arch/x86/entry/syscalls/syscall_32.tbl:35:20       i386    getpid                  sys_getpid
```
Das sind jeweils die Einträge für 64 Bit und 32 Bit Systeme. Hier wird der System-Call-Wrapper mit dem eigentlichen System-Call verknüpft, dessen Code im Kernel zu finden ist.

### Definition des System-Calls im Kernel
Im Kernel suchen wir also sys_getpid.

**Befehl:**
```bash
(base) root@DESKTOP-OL5SHQL:/usr/src/glibc/linux# grep -rnw 'sys_getpid' kernel/sys.c
```

```c
959: * sys_getpid - return the thread group id of the current process
```

Ein größerer Ausschnit mit der Methodendeklaration:

```c
/**
 * sys_getpid - return the thread group id of the current process
 *
 * Note, despite the name, this returns the tgid not the pid.  The tgid and
 * the pid are identical unless CLONE_THREAD was specified on clone() in
 * which case the tgid is the same in all threads of the same group.
 *
 * This is SMP safe as current->tgid does not change.
 */
SYSCALL_DEFINE0(getpid)
{
        return task_tgid_vnr(current);
}

/* Thread ID - the internal kernel "pid" */
```

Hier sehen wir sys_getpid im Kernel mit seiner Funktion.

Wir sehen, dass getpid einfach return task_tgid_vnr(current); ausführt. Die Funktion task_tgid_vnr(current) gibt die TGID des aktuellen Prozesses zurück. Diese TGID ist normalerweise gleich der Prozess-ID (PID) des Prozesses.


### Fazit

Als Fazit schließe ich aus der Aufgabe, dass der Ablauf vom System-Call-Wrapper zum eigentlichen System-Call im Kernel einem klar strukturierten Schema folgt: 
Die System-Call-Wrapper, wie beispielsweise getpid(), sind häufig in Dateien wie /usr/include/unistd.h definiert und bieten eine benutzerfreundliche Schnittstelle für Entwickler. 
Diese Wrapper sind über die System-Call-Tabellen, die sich im Verzeichnis /linux/arch/x86/entry/syscalls/ befinden, mit den entsprechenden Kernel-Implementierungen verknüpft. 
Die Tabellen syscall_32.tbl und syscall_64.tbl ordnen die Wrapper den jeweiligen System-Call-Funktionen im Kernel zu und stellen so eine Verbindung zwischen der Benutzer- und der Kernel-Ebene her. 
Dieses Prinzip ist ein zentraler Bestandteil des Linux-Betriebssystems und ermöglicht eine effiziente Nutzung der Systemressourcen.

## Aufgabe 2
**Aufgabenstellung:**
Denken Sie über einem experimentellen Ansatz nach, die minimale Latenz bei der Ausführung eines System-Calls zu ermitteln. 
Entwerfen Sie ein geeignetes Programm, um diese minimale Latenz zu bestimmen. Diskutieren Sie das Ergebnis. 

Für die Analyse der minimalen Latenz eines System-Calls habe ich einen Code geschrieben. Den Code finden Sie in der Datei syscall_latency.c in diesem Repository.
Der Code führt 1 000 000 mal den Systemcall getpid() aus und misst jeweils die Zeit. Die niedrigste Zeit aller Iterationen wird gespeichert. 

Bei mir kam auf meinem Laptop konsistent etwas zwischen 95 Nanosekunden und 98 Nanosekunden raus. 
Aus eigenem Interesse habe ich den gleichen Code nochmal ausgeführt, aber nicht getpid(), sondern syscall(SYS_getpid) ausgeführt, was den System-Call direkt im Kernen ausführt.
Die Zeit hat sich bei mir nicht geändert. Das heißt, dass der System-Call-Wrapper sehr effizient arbeitet.

## Aufgabe 3
**Aufgabenstellung:**
Ziel dieser Aufgabe soll es sein, die durchschnittliche Dauer eines Kontextwechsels auf einem System Ihrer Wahl empirisch zu 
ermitteln. Denken Sie darüber nach, was ein Kontextwechsel im Detail ist, wie Sie die Kontextwechselzeiten überhaupt messen 
und wie Sie mittels eines von Ihnen realisierten Programms diese Zeiten tatsächlich auch bestimmen können. Recherchieren Sie 
anschließend, mit welchen Standardwerkzeugen (z.B. für das Profiling von Anwendungen) Kontextwechselzeiten ebenfalls 
ermitteln werden können. Vergleichen Sie diese Zeiten mit Ihren ermittelten Werten und versuchen Sie eventuelle Diskrepanzen 
zu begründen. Zusätzlich zu den unmittelbaren Zeitkosten eines Kontextwechsels, die sich aus der Ausführung der notwendigen 
Aufgaben direkt ergeben, entstehen auch indirekte Kosten, die sich negativ auf die Anwendungsperformanz auswirken und die 
nicht leicht quantifizierbar sind. Gehen Sie im Ergebnisbericht zu dieser Aufgabe auch auf diese indirekten Kosten ein und 
versuchen Sie abzuschätzen, welche Auswirkungen sich daraus für die Effizienz der Anwendungsausführung ergeben. 

In dieser Aufgabe analysieren wie die Zeit eines Kontextwechsels. Zu beginn möchte ich den Begriff definieren:

*Ein Kontextwechsel ist der Vorgang, bei dem das Betriebssystem die CPU von einem Prozess oder Thread zu einem anderen wechselt. Dabei wird der Zustand des aktuellen Prozesses gespeichert und der Zustand des neuen Prozesses geladen. Dies ermöglicht Multitasking, hat aber auch einen gewissen Overhead, der die Leistung beeinflussen kann.*

Um die Kosten des Kontextwechsels zu messen, habe ich zwei Programme geschrieben. Die Programme haben jeweils zwei Threads indem sie hochzählen. In einem Programm wird nach jeder Zahl der Thread gewechselt um Kontextwechsel zu provozieren. Der andere Code ist meine Kontrollgruppe. Ich messe die Zeiten, die beide brauchen und kann die Zeiten dann vergleichen.
Ein paar Ergebnisse:
| Programm          | Context Switches | Ausführungszeit (Nanosekunden) |
|-------------------|------------------|--------------------------------|
| context_switch    | Ja               | 19177000                       |
| context_switch    | Ja               | 108247000                      |
| context_switch    | Ja               | 102581000                      |
| context_switch    | Ja               | 105654000                      |
| context_switch    | Ja               | 105546000                      |
| control_group     | Nein             | 485000                         |
| control_group     | Nein             | 824000                         |
| control_group     | Nein             | 449000                         |
| control_group     | Nein             | 604000                         |
| control_group     | Nein             | 7197000                        |

Kurze Auswertung:
Die Programme mit Context-Switches benötigen im Durchschnitt 88.241.000 Nanosekunden, während die Programme ohne Context-Switches nur 1.911.000 Nanosekunden benötigen.

Fazit:
Die Analyse zeigt deutlich, dass die Ausführungszeit der Programme mit Context-Switches im Durchschnitt signifikant höher ist als die der Programme ohne Context-Switches. Tatsächlich dauert es im Schnitt etwa 46-mal länger (88.241.000 / 1.911.000 ≈ 46), wenn Context-Switches involviert sind. Diese zusätzliche Zeit wird durch den Overhead verursacht, der beim Wechseln zwischen verschiedenen Prozessen und Threads entsteht. Daher ist es oft vorteilhaft, Context-Switches zu minimieren, insbesondere in zeitkritischen Anwendungen, um die Leistung zu optimieren.


