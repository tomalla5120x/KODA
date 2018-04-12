# KODA
## Konfigurowanie projektu
1. Ściągnij odpowiednią wersję OpenCV:
  * w przypadku Visual 2017 lub 2015, ściągnij wersję 3.4.0
  * w przypadku Visual 2013, ściągnij wersję 3.1.0
2. Sklonuj to repozytorium.
3. Następnie skopiuj plik `KODA\KODA\PropertySheet.props_original` i wklej w tym samym folderze z nazwą zmienioną na `PropertySheet.props`.
4. W pliku `PropertySheet.props` są następujące parametry, którze należy zaktualizować:
  * parametr `OpenCVPath` to ścieżka do katalogu z wypakowaną biblioteką OpenCV
  * parametr `OpenCVLib` to względna ścieżka do pliku *.lib biblioteki OpenCV. Należy wykorzystać plik zgodny z wersją Visual Studio. Folder vc12 odpowiada wersji Visual 2013, vc14 to 2015, a vc15 to 2017
  * parametr `OpenCVLibDebug` jest analogiczny jak wyżej - tyle że to wersji pliku *.lib dla wersji debug (z sufiksem `d`, na przykład: opencv_world310d.lib)
5. Żeby uruchomić skompilowany program wymagana jest jeszcze biblioteka `<folder OpenCV>\build\x64\vc<wersja>\bin\opencv_world<wersja>(d).dll` (plik z 'd' jest używany w wersji Debug). Najprościej jest ją skopiować do folderu `KODA\KODA`, ew. dodać ścieżkę do pliku DLL do zmiennej środowiskowej `PATH`.

## Uwagi dotyczące implementacji

Projekt na tę chwilę jest w pełni funkcjonalny, działa i się kompiluje. Aktualnie zaimplementowane algorytmy (de)kompresji są prymitywne - po prostu przepisują wszystkie piksele z wejścia na wyjście i na odwrót. Po ich zaimplementowaniu można skompilować projekt i je przetestować.

1. Sposób uruchamiania programu (przykład, szczegóły w manualu programu):
  * kompresja:
    `KODA -i "..\..\data\natural\barbara.pgm" -o "compressed_barbara.dat" -c 1`
  * dekompresja:
    `KODA -i "compressed_barbara.dat" -o decompressed_barbara.pgm`

2. Wasze algorytmy (de)kompresji powinny być umieszczone w plikach `codec_classic/block/markov.cpp`

3. Cały plik zdekompresowany ma charakter warstwowy. To znaczy, że ja dodaję swój własny nagłówek, a wy dopiero swoje.
   Mój nadrzędny etap operuje na poziomie samego rysunku i dokonuje ekstrakcji pikseli. Wy operujecie już na poziomie strumienia
   bajtów i nie musicie w ogóle wiedzieć że te dane są pikselami jakiegokolwiek obrazu.

4. Ja widzę to tak, że wy najpierw zbudujecie model na podstawie danych wejściowych, do strumienia wyjściowego zrzucicie powstałe drzewo
   lub drzewa Huffmana (jako długości kolejnych słów kodowych w alfabecie), a bezpośrednio po nich będą skompresowane dane jako ciąg bitów.
   * CLASSIC - nagłówek to drzewo Huffmana z 256 liściami, czyli nagłówek będzie miał 256 bajtów (po jednym bajcie na jeden symbol).
   * BLOCK - nagłówek to drzewo Huffmana z 65536 liściami, każde słowo kodowe jest zakodowane przy użyciu dwóch bajtów, czyli
     nagłówek będzie miał 131072 bajty
   * MARKOV - nagłówek to 256 drzew Huffmana, każdy po 256 liści. Każdy kodowany symbol ma swój kontekst, którym jest poprzedni symbol.
     Każde z 256 drzew Huffmana odpowiada konkretnemu kontekstowi. Czyli nagłówek będzie miał 65536 bajtów.

5. W ogólności funkcje do (de) kompresji zwracają wartość TRUE jeżeli algorytm zakończył się powodzeniem a false w przeciwnym wypadku.
   Dodatkowo funkcja do kompresji zwraca trzy dodatkowe wartości: entropia modelu, średnią
   długość bitową słów kodowych oraz długość nagłówka, jaką dodacie przed samymi skompresowanymi danymi (jest to używane do wyznaczenia efektywności
   kompresji). Odnośnie liczenia entropii: robi to generalnie klasa HuffmanTreeBuilder.
   * CLASSIC - nic nie trzeba zmieniać
   * BLOCK - entropię nalezy jeszcze podzielić przez 2
   * MARKOV - entropię każdego z 256 kontekstów należy jeszcze połączyć - wzór jest na slajdach wykładowych

6. Najbardziej podstawowe błędy (w szczególności podczas dekompresji) powinny być wykrywane i odpowiednio obsługiwane.
   Przykład takiego błędu: niespodziewany koniec strumienia na wejściu podczas dekompresji, np. w pewnym momencie brakuje kolejnych bitów do odkodowania.
   Jeżeli jakiś błąd zostanie wykryty, nalezy na CERR wypisać komunikat z błędem. Sama funkcja natomiast powinna zwrócić FALSE.

7. Funkcja do (de)kompresji przyjmuje liczbę bajtów oryginalnego strumienia danych. Algorytm (de)kompresji więc wie dokładnie ile symboli jest do za/odkodowania.

8. Do zapisywania/odczytywania dwubajtowych zmiennych całkowitoliczbowych należy użyć funkcji `Utils::uint32Write/Read`
   Standaryzują one formę zapisu zmiennej do pliku (jako little endian).

9. Żeby w ramach debugowania podejrzeć binarną reprezentację jakiegoś słowa kodowego (instancja Code) można ją po prostu przekierować na cout, np.:
   ```
   Code code = ...;
   cout << code;
   ```
   Automatycznie zostanie wszystko sformatowane i w czytelnej postaci wyświetlone.

10. Najpierw należy uzyskać listę długości wszystkich słów kodowych - jest ona wczytywana albo z nagłówka dekompresowanego pliku (dekompresja) albo za pomocą
   klasy `HuffmanTreeBuilder` (kompresja), do której dodaje się po jednym symbolu, a który na końcu zwraca zbudowany model.
   Taką listę przekazuje się następnie do klasy `HuffmanTreeEncoder` (kompresja) albo `HuffmanTreeDecoder`(dekompresja).
   `HuffmanTreeEncoder` - proste odpytywanie. Dajesz symbol, zwraca słowo kodowe (instancja `Code`)
   `HuffmanTreeDecoder` - przechodzone jest drzewo bit po bicie. Dla każdego węzła: może być albo liściem i zawierać końcowy symbol, albo zawierać rozgałęzienie
   na zero i jedynkę.

   Przykład:

   ```
   vector<unsigned int> bitLengths({ 3, 3, 6, 7, 7, 5, 5, 8, 9, 9, 8, 10, 11, 12, 14, 14, 13, 10, 11, 12, 12, 6, 5, 5, 6, 6, 5, 4, 4, 3, 4, 5, 5, 4, 9,
      9, 8, 7, 9, 10, 10, 8, 8, 9, 9, 7, 9, 10, 10, 8, 7, 10, 11, 12, 12, 10, 11, 12, 12, 9, 9 });

   HuffmanTreeDecoder decoder(bitLengths);
   auto node = decoder.getRoot();
   // w ogólności po każdym bicie należy sprawdzać czy węzeł jest liściem
   // tutaj przykład szukania symbolu dla ciągu bitów: 1111 1101 11
   cout << node->traverseOne()->traverseOne()->traverseOne()->traverseOne()->
      traverseOne()->traverseOne()->traverseZero()->traverseOne()->
      traverseOne()->traverseOne()->getSymbol() << endl;

   HuffmanTreeEncoder encoder(bitLengths);
   // zdobycie słowa kodowego dla symbolu 17
   cout << encoder.getCode(17) << endl;
   ```