# Projekt 1  

## Instrukcje uruchomienia projektu  

1. Aby uruchomić program dla pierwszego projektu, przejdź do folderu `Zad_1/src/output` i uruchom program `zad_1.exe`.  

2. Program wymaga podania 4 argumentów w następującej kolejności:  

   - Liczba filozofów.  

   - Liczba milisekund, przez które filozof będzie myślał. Filozof będzie jadł przez dwa razy dłuższy czas.  

   - Liczba iteracji pętli działań każdego filozofa (myślenie, próba podniesienia widelców, jedzenie i odkładanie widelców).
   Można ustawić nieskończoną liczbę iteracji, podając *-1* jako argument.  

   - Bit flagi decydującej o wypisywaniu komunikatów konsolowych dla podnoszenia i odkładania widelców.  

## Opis problemu

**Problem ucztujących filozofów** to model problemu synchronizacji procesów.

Problem opisuje kilku filozofów siedzi przy okrągłym stole. Każdy filozof na zmianę myśl lub je. Między każdym z nich leży jeden widelec (jest ich tyle samo co filozofów). Aby filozof mógł jeść, musi wziąć dwa widelce - jeden ze swojej lewej i jeden z prawej.

Taki układ powoduje możliwość pojawienia się problemów takich jak zakleszczenie (ang. deadlock) - tj. sytuacja w której każdy z filozofów podniesie jedne widelec przez co żaden z nich nie bedzie mógł się posilic i oddać widelców - lub zagłodzenie (ang. starvation) - tj. sytuacja gdy któryś z filozofów wyprzedzany przez innych nie może się posilic przez dłuższy czas.

W tym modelu filozofowie reprezentują wątki, a widelce zasoby współdzielone. 

## Opis programu

Program napisałem z uzyciem biblioteki `pthreads`.

W programie wątki reprezentują filozofów. Każdy z nich wykonuje pętlę działań - kolejno:

1. myśli przez pewien czas
2. próbuje wziąć pierwszy widelec
3. próbuje wziąć drugi widelec
4. je przez pewien czas
5. odkłada pierwszy widelec
6. odkłada drugi widelec

Widelce zostały zamodelowane z użyciem tablicy mutexów `pthread_mutex_t`, co zapewnia, że gdy widelec jest podniesiony przez jednego filozofa inny nie będzie mógł go uzyć.

W celu zapobiegania zakleszczaniu został uzyty mechanizm hierarchicznego dostepu do widelców - każdy filozof musi najpierw wziąc widelec o nizszym indeksie, a nastepnie ten o wyższym.
