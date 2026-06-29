import matplotlib.pyplot as plt

from dataset import load_datasets
from models import create_basic_cnn


def plot_training_history(history):
    plt.figure()
    plt.plot(history.history["accuracy"], label="Train accuracy")
    plt.plot(history.history["val_accuracy"], label="Validation accuracy")
    plt.xlabel("Epoha")
    plt.ylabel("Točnost")
    plt.legend()
    plt.title("CNN 1 - Točnost tijekom treniranja")
    plt.savefig("results/cnn1_accuracy.png")
    plt.close()

    plt.figure()
    plt.plot(history.history["loss"], label="Train loss")
    plt.plot(history.history["val_loss"], label="Validation loss")
    plt.xlabel("Epoha")
    plt.ylabel("Gubitak")
    plt.legend()
    plt.title("CNN 1 - Gubitak tijekom treniranja")
    plt.savefig("results/cnn1_loss.png")
    plt.close()


def main():
    image_size = (128, 128)
    batch_size = 32
    epochs = 10

    train_dataset, validation_dataset = load_datasets(
        image_size=image_size,
        batch_size=batch_size
    )

    model = create_basic_cnn(input_shape=(128, 128, 3))

    model.compile(
        optimizer="adam",
        loss="binary_crossentropy",
        metrics=["accuracy"]
    )

    model.summary()

    history = model.fit(
        train_dataset,
        validation_data=validation_dataset,
        epochs=epochs
    )

    validation_loss, validation_accuracy = model.evaluate(validation_dataset)

    print("\nRezultati osnovnog CNN modela:")
    print(f"Validation loss: {validation_loss:.4f}")
    print(f"Validation accuracy: {validation_accuracy:.4f}")

    plot_training_history(history)

    model.save("results/cnn1_basic_model.keras")


if __name__ == "__main__":
    main()