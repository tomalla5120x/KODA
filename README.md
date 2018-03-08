# KODA
## Konfigurowanie projektu
1. Ściągnij [OpenCV 3.4.0](https://sourceforge.net/projects/opencvlibrary/files/opencv-win/3.4.0/opencv-3.4.0-vc14_vc15.exe/download) i wypakuj zawartość gdzieś na dysku.
2. Sklonuj to repozytorium. Następnie zmodyfikuj plik `KODA\KODA\PropertySheet.props` i zaktualizuj swoją ścieżkę do katalogu OpenCV (oryginalna ścieżka to `E:\OpenCV`).
3. Żeby uruchomić skompilowany program wymagana jest jeszcze biblioteka `<folder OpenCV>\build\x64\vc14\bin\opencv_world340(d).dll` (plik z 'd' jest używany w wersji Debug). Najprościej jest ją skopiować do folderu `KODA\KODA`, ew. dodać ścieżkę do pliku DLL do zmiennej środowiskowej `PATH`.

Póki co to powinno wystarczyć. Minus takiej konfiguracji jest taki, że plik `PropertySheet.props` jest częścią repozytorium, więc trzeba uważać by go nie commitować i nie pobierać. Na razie zrobimy tak: wszyscy skonfigurują sobie środowisko, a potem ten plik po prostu skasujemy z repozyzorium i dodamy do .gitignore. Jeżeli ktoś ma lepszy pomysł, krzyczeć! :)