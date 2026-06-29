from tensorflow.keras import Sequential
from tensorflow.keras.layers import (
    Conv2D,
    MaxPooling2D,
    Flatten,
    Dense,
    Dropout,
    GlobalAveragePooling2D,
    Rescaling
)
from tensorflow.keras.applications import MobileNetV2


def create_basic_cnn(input_shape=(128, 128, 3)):
    """
    Osnovni CNN model za binarnu klasifikaciju slika pasa i mačaka.
    """

    model = Sequential(name="CNN_1_basic")

    model.add(Conv2D(
        filters=32,
        kernel_size=(3, 3),
        activation="relu",
        input_shape=input_shape
    ))

    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Conv2D(
        filters=64,
        kernel_size=(3, 3),
        activation="relu"
    ))

    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Flatten())

    model.add(Dense(
        units=128,
        activation="relu"
    ))

    model.add(Dense(
        units=1,
        activation="sigmoid"
    ))

    return model


def create_improved_cnn(input_shape=(128, 128, 3)):
    """
    Poboljšani CNN model za binarnu klasifikaciju slika pasa i mačaka.

    U odnosu na osnovni model dodano je:
    - treći konvolucijski blok
    - Dropout sloj radi smanjenja overfittinga
    """

    model = Sequential(name="CNN_2_improved")

    model.add(Conv2D(
        filters=32,
        kernel_size=(3, 3),
        activation="relu",
        input_shape=input_shape
    ))

    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Conv2D(
        filters=64,
        kernel_size=(3, 3),
        activation="relu"
    ))

    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Conv2D(
        filters=128,
        kernel_size=(3, 3),
        activation="relu"
    ))

    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Flatten())

    model.add(Dense(
        units=128,
        activation="relu"
    ))

    model.add(Dropout(0.3))

    model.add(Dense(
        units=1,
        activation="sigmoid"
    ))

    return model


def create_transfer_learning_cnn(input_shape=(128, 128, 3)):
    """
    CNN 3 model temeljen na transfer learning pristupu.

    Koristi MobileNetV2 mrežu prethodno treniranu na ImageNet skupu podataka.
    Osnovni MobileNetV2 dio koristi se kao izdvajac znacajki slike,
    dok se na kraj dodaje vlastiti klasifikacijski dio za dvije klase:
    pas i macka.
    """

    base_model = MobileNetV2(
        input_shape=input_shape,
        include_top=False,
        weights="imagenet"
    )

    # Zamrzavamo prethodno naucene slojeve.
    # Time se u pocetnoj fazi trenira samo nas klasifikacijski dio.
    base_model.trainable = False

    model = Sequential(name="CNN_3_MobileNetV2_transfer_learning")

    # dataset.py vec normalizira slike na raspon 0-1.
    # MobileNetV2 ocekuje vrijednosti u rasponu -1 do 1,
    # pa ih ovdje dodatno skaliramo.
    model.add(Rescaling(
        scale=2.0,
        offset=-1.0,
        input_shape=input_shape
    ))

    model.add(base_model)

    model.add(GlobalAveragePooling2D())

    model.add(Dense(
        units=128,
        activation="relu"
    ))

    model.add(Dropout(0.3))

    model.add(Dense(
        units=1,
        activation="sigmoid"
    ))

    return model