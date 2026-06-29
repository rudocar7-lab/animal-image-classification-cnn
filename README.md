# Klasifikacija slika životinja pomoću konvolucijskih neuronskih mreža

Ovaj projekt izrađen je kao praktični dio završnog rada na temu **Klasifikacija slika životinja temeljena na konvolucijskim neuronskim mrežama**. Cilj projekta je razviti program koji na temelju ulazne slike određuje nalazi li se na slici pas ili mačka.

Za izradu modela korišten je programski jezik Python te biblioteke TensorFlow i Keras. Modeli su trenirani i validirani na dijelu skupa podataka **Kaggle Cats vs Dogs**.

## Korišteni modeli

U projektu su implementirana tri modela:

1. **CNN1** – osnovni konvolucijski model s dva konvolucijska bloka.
2. **CNN2** – poboljšani CNN model s dodatnim konvolucijskim slojem i Dropout regularizacijom.
3. **CNN3** – model temeljen na MobileNetV2 arhitekturi i transfer learning pristupu.

## Struktura projekta

```text
ZAVRSNI_RAD/
├── dataset/
│   ├── train/
│   │   ├── cats/
│   │   └── dogs/
│   └── validation/
│       ├── cats/
│       └── dogs/
├── results/
├── dataset.py
├── models.py
├── prepare_small_dataset.py
├── train.py
├── train2.py
└── train3.py
```

## Opis datoteka

* `prepare_small_dataset.py` – priprema skup podataka, provjerava ispravnost slika i sprema ih u odgovarajuće mape za treniranje i validaciju.
* `dataset.py` – učitava trening i validacijski skup podataka pomoću TensorFlow/Keras funkcija.
* `models.py` – sadrži definicije svih korištenih modela.
* `train.py` – trenira osnovni CNN model.
* `train2.py` – trenira poboljšani CNN model.
* `train3.py` – trenira MobileNetV2 transfer learning model.
* `results/` – mapa u koju se spremaju trenirani modeli i grafovi točnosti i funkcije gubitka.

## Priprema okruženja

Prije pokretanja potrebno je imati instaliran Python i potrebne biblioteke. Preporučuje se korištenje virtualnog okruženja.

Izrada virtualnog okruženja:

```powershell
python -m venv venv
```

Aktivacija virtualnog okruženja u Windows PowerShellu:

```powershell
.\venv\Scripts\Activate.ps1
```

Instalacija potrebnih biblioteka:

```powershell
pip install tensorflow matplotlib pillow
```

## Priprema skupa podataka

Prije treniranja modela potrebno je pripremiti skup podataka. Izvorni skup slika treba biti smješten u mapu:

```text
C:\Users\Korisnik\Desktop\PetImages
```

U toj mapi trebaju postojati podmape:

```text
Cat/
Dog/
```

Nakon toga pokreće se skripta za pripremu podataka:

```powershell
python prepare_small_dataset.py
```

Skripta stvara mapu `dataset` i unutar nje organizira slike u trening i validacijski skup:

```text
dataset/train/cats
dataset/train/dogs
dataset/validation/cats
dataset/validation/dogs
```

Tijekom pripreme oštećene ili neispravne slike se preskaču.

## Pokretanje treniranja

### Treniranje osnovnog CNN modela

```powershell
python train.py
```

### Treniranje poboljšanog CNN modela

```powershell
python train2.py
```

### Treniranje MobileNetV2 modela

```powershell
python train3.py
```

## Rezultati

Nakon treniranja modela u mapu `results` spremaju se:

* trenirani modeli u `.keras` formatu,
* grafovi točnosti tijekom treniranja,
* grafovi funkcije gubitka tijekom treniranja.

Rezultati se koriste za usporedbu modela i analizu overfittinga.

## Napomena

MobileNetV2 model pri prvom pokretanju može preuzeti prethodno naučene težine s interneta. Nakon toga se model može koristiti kao izdvajatelj značajki za klasifikaciju slika pasa i mačaka.

## Autor

Rudi Maćešić
Sveučilište u Rijeci, Tehnički fakultet
Sveučilišni prijediplomski studij računarstva
