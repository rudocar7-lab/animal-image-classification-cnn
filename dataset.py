import tensorflow as tf


def load_datasets(
    train_dir="dataset/train",
    validation_dir="dataset/validation",
    image_size=(128, 128),
    batch_size=32
):
    train_dataset = tf.keras.utils.image_dataset_from_directory(
        train_dir,
        image_size=image_size,
        batch_size=batch_size,
        label_mode="binary"
    )

    validation_dataset = tf.keras.utils.image_dataset_from_directory(
        validation_dir,
        image_size=image_size,
        batch_size=batch_size,
        label_mode="binary"
    )

    normalization_layer = tf.keras.layers.Rescaling(1.0 / 255)

    train_dataset = train_dataset.map(
        lambda images, labels: (normalization_layer(images), labels)
    )

    validation_dataset = validation_dataset.map(
        lambda images, labels: (normalization_layer(images), labels)
    )

    return train_dataset, validation_dataset