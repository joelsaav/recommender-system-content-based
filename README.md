# Sistema de Recomendación Basado en Contenido - Guía de Uso

## Compilación

Desde el directorio raíz del proyecto (Windows):

```powershell
g++ -std=c++11 src/document.cc src/documentManager.cc src/tools.cc src/main.cc -Iinclude -o recommender-system.exe
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
- **Term**: El término (palabra lematizada)
- **TF**: Frecuencia del término en el documento (número de apariciones)
- **IDF**: Inverse Document Frequency (importancia del término en el corpus)
- **TF-IDF**: Términos vectorizados y normalizados por documento
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

### 2. Matriz de Similitud Coseno

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

## Opciones de Línea de Comandos

- `-d <archivos...>`: Uno o más documentos de texto a analizar (requerido)
- `-s <archivo>`: Archivo con stop-words (requerido)
- `-l <archivo>`: Archivo JSON con reglas de lematización (requerido)
- `-h` o `--help`: Muestra ayuda

## Notas 

**TF (Term Frequency)**:
```
TF(t,d) = número de veces que el término t aparece en el documento d (1 + log10(t))
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
TF-IDF(t,d) = Términos vectorizados y normalizados por documento
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
4. **Stop-words**: Se eliminan palabras vacías (se marcan con otro carácter para mantener proporciones)
5. **Lematización**: Se aplican reglas de reducción a formas base

### Características del Vocabulario

- Solo se incluyen términos que aparecen en al menos un documento
- Los términos se indexan alfabéticamente
- Se ignoran los placeholders (stop-words) en todos los cálculos

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
