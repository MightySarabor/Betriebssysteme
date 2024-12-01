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

