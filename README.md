# Sistema de Recomendación Basado en Contenido - Guía de Uso

## Compilación

Desde el directorio raíz del proyecto:

```bash
make
```
Para limpìar los archivos resultantes de la compilación:
```bash
make clean
```
## Ejecución
### Opciones de Línea de Comandos

- `-d <archivos...>`: Uno o más documentos de texto a analizar (requerido)
- `-s <archivo>`: Archivo con stop-words (requerido)
- `-l <archivo>`: Archivo JSON con reglas de lematización (requerido)
- `-h` o `--help`: Muestra ayuda

### Ejemplo básico (1 documento)
```bash
.\recommender-system-content-based -d documents/document-01.txt -s stop-words/stop-words-en.txt -l lemmatization/corpus-en.json
```

### Ejemplo con múltiples documentos
```bash
.\recommender-system-content-based -d documents/document-01.txt documents/document-02.txt documents/document-03.txt -s stop-words/stop-words-en.txt -l lemmatization/corpus-en.json
```

```bash
.\recommender-system-content-based -d documents/document-01.txt documents/document-02.txt documents/document-03.txt documents/document-04.txt documents/document-05.txt -s stop-words/stop-words-en.txt -l lemmatization/corpus-en.json
```

## Salida del Programa

El programa genera:

### Tabla TF-IDF por Documento

Para cada documento procesado, se genera una tabla con las siguientes columnas:
- **Term**: El término (palabra lematizada)
- **TF**: Frecuencia del término en el documento
- **IDF**: Inverse Document Frequency (importancia del término en el corpus)
- **TF-IDF**: Vector de las frecuecias normalizadas del término
- **Index**: Índice del término en el vocabulario (fila, columna)

Ejemplo:
```
=========================== documents/document-03.txt ==========================

Term                                    TF         IDF       TFIDF          Index
------------------------------------------------------------------------------------------
a                                 2.397940    0.000000    0.133168          0, 50
accept                            0.000000    0.477121    0.000000            N/A
acceptance                        1.000000    0.000000    0.055534         11, 40
achieve                           0.000000    0.477121    0.000000            N/A
...
```

### Matriz de Similaridad Coseno

Se genera una matriz simétrica NxN (donde N = número de documentos) que muestra la similitud entre cada par de documentos. Los valores van de 0 (completamente diferentes) a 1 (idénticos).

Ejemplo:
```
=========================== COSINE SIMILARITY MATRIX ===========================

                 Doc 1       Doc 2       Doc 3
--------------------------------------------------------------------------------
      Doc 1:     1.000000    0.670136    0.655077
      Doc 2:     0.670136    1.000000    0.641121
      Doc 3:     0.655077    0.641121    1.000000
```

## Notas 

**TF (Term Frequency)**:
```
TF(t,d) = número de veces que el término t aparece en el documento d (1 + log10(tf))
```

**IDF (Inverse Document Frequency) con smoothing**:
```
IDF(t) = log10(N / DF(t))
donde:
  N = número total de documentos
  DF(t) = número de documentos que contienen el término t
```

**TF-IDF**:
```
TF-IDF(t,d) = Vector de las frecuencias de los términos normalizadas
```

**Similitud Coseno**:
```
cos(d1, d2) = Producto escalar de los vectores TF-IDF del par de documentos a calcular.
```

### Preprocesamiento

1. **Preprocesamiento**: Se eliminan signos de puntuación de inicio y fin de cada término y se convierten a minúsculas.
2. **Stop-words**: Se eliminan palabras vacías (se marcan con otro carácter para mantener proporciones).
3. **Lematización**: Se mapean términos relacionados morfológicamente a un término común.


## Estructura del Proyecto

```
recommender-system-content-based/
├── documents/          # Documentos de texto a analizar
├── stop-words/         # Archivos con palabras vacías
├── lemmatization/      # Archivos JSON con reglas de lematización
├── include/            # Headers (.h)
│   ├── document.h
│   ├── documentManager.h
│   └── tools.h
├── src/                # Código fuente (.cc)
    ├── document.cc
    ├── documentManager.cc
    ├── tools.cc
    └── main.cc
```
