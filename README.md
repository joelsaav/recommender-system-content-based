# Sistema de Recomendación Basado en Contenido - Guía de Uso

## Compilación

Desde el directorio raíz del proyecto (Windows):

```powershell
g++ -std=c++11 src/file.cc src/tools.cc src/main.cc -Iinclude -o recommender-system.exe
```

## Ejecución

### Ejemplo básico (1 documento)
```powershell
.\recommender-system.exe -d documents/document-01.txt -s stop-words/stop-words-en.txt -l lemmatization/corpus-en.json
```

### Ejemplo con múltiples documentos
```powershell
.\recommender-system.exe -d documents/document-01.txt documents/document-02.txt documents/document-03.txt -s stop-words/stop-words-en.txt -l lemmatization/corpus-en.json
```

```powershell
.\recommender-system.exe -d documents/document-01.txt documents/document-02.txt documents/document-03.txt documents/document-04.txt documents/document-05.txt -s stop-words/stop-words-en.txt -l lemmatization/corpus-en.json
```

## Salida del Programa

El programa genera dos tipos de salida:

### 1. Tablas TF-IDF por Documento

Para cada documento procesado, se genera una tabla con las siguientes columnas:
- **Index**: Índice del término en el vocabulario
- **Term**: El término (palabra lematizada)
- **TF**: Frecuencia del término en el documento (número de apariciones)
- **IDF**: Inverse Document Frequency (importancia del término en el corpus)
- **TF-IDF**: Producto TF × IDF (peso del término en el documento)

Ejemplo:
```
==========================================================================================
TF-IDF TABLE FOR: documents/document-01.txt
==========================================================================================
Index   Term                TF        IDF            TF-IDF
------------------------------------------------------------------------------------------
189     lake                13        0.000000       0.000000
397     walk                9         0.000000       0.000000
425     write               6         0.000000       0.000000
405     water               6         0.000000       0.000000
172     i                   44        0.000000       0.000000
...
```

### 2. Matriz de Similitud Coseno

Se genera una matriz simétrica NxN (donde N = número de documentos) que muestra la similitud entre cada par de documentos. Los valores van de 0 (completamente diferentes) a 1 (idénticos).

Ejemplo:
```
================================================================================
COSINE SIMILARITY MATRIX
================================================================================
                    Doc 0          Doc 1          Doc 2
--------------------------------------------------------------------------------
Document 0          1.000000       0.073397       0.055204
Document 1          0.073397       1.000000       0.073705
Document 2          0.055204       0.073705       1.000000
================================================================================
```

## Opciones de Línea de Comandos

- `-d <archivos...>`: Uno o más documentos de texto a analizar (requerido)
- `-s <archivo>`: Archivo con stop-words (requerido)
- `-l <archivo>`: Archivo JSON con reglas de lematización (requerido)
- `-h` o `--help`: Muestra ayuda

## Notas 

**TF (Term Frequency)**:
```
TF(t,d) = número de veces que el término t aparece en el documento d
```

**IDF (Inverse Document Frequency) con smoothing**:
```
IDF(t) = log((N + 1) / (DF(t) + 1))
donde:
  N = número total de documentos
  DF(t) = número de documentos que contienen el término t
```

**TF-IDF**:
```
TF-IDF(t,d) = TF(t,d) × IDF(t)
```

**Similitud Coseno**:
```
cos(d1, d2) = (d1 · d2) / (||d1|| × ||d2||)
donde:
  d1 · d2 = producto punto de los vectores TF-IDF
  ||d|| = norma euclidiana del vector
```

### Preprocesamiento

1. **Tokenización**: Se separa el texto por espacios
2. **Limpieza**: Se eliminan signos de puntuación de inicio y fin de cada token
3. **Normalización**: Todo se convierte a minúsculas
4. **Stop-words**: Se eliminan palabras vacías (se marcan como "---")
5. **Lematización**: Se aplican reglas de reducción a formas base

### Características del Vocabulario

- Solo se incluyen términos que aparecen en al menos un documento
- Los términos se indexan alfabéticamente
- Se ignoran los placeholders "---" (stop-words) en todos los cálculos

## Estructura del Proyecto

```
recommender-system-content-based/
├── documents/          # Documentos de texto a analizar
├── stop-words/         # Archivos con palabras vacías
├── lemmatization/      # Archivos JSON con reglas de lematización
├── include/            # Headers (.h)
│   ├── file.h
│   └── tools.h
├── src/                # Código fuente (.cc)
   ├── file.cc
   ├── tools.cc
   └── main.cc
```
