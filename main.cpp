#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QFile>
#include <QByteArray>
#include <QFileInfo>
#include <QTimer>
#include <QTime>
#include <QRadioButton>
#include <QVBoxLayout>


void xorWithKey(QByteArray &data, const QByteArray &key) {
    for (int i = 0; i < data.size(); i++) {
        data[i] = data[i] ^ key[i % key.size()];
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow window;
    window.setWindowTitle("XOR File Encryptor");

    // Создаем и настраиваем виджеты GUI
    QLabel *inputLabel = new QLabel("Выберите входной файл:");
    QLineEdit *inputFileLineEdit = new QLineEdit;
    QPushButton *inputFileButton = new QPushButton("Обзор...");

    QLabel *outputLabel = new QLabel("Выберите выходной файл:");
    QLineEdit *outputFileLineEdit = new QLineEdit;
    QPushButton *outputFileButton = new QPushButton("Обзор...");

    QLabel *keyLabel = new QLabel("Введите 8-байтовый ключ (в шестнадцатеричной форме):");
    QLineEdit *keyLineEdit = new QLineEdit;

    QPushButton *encryptButton = new QPushButton("Зашифровать");

    QRadioButton *singleRunRadioButton = new QRadioButton("Разовый запуск");
    QRadioButton *timerRunRadioButton = new QRadioButton("Запуск по таймеру");
    QLineEdit *timerIntervalLineEdit = new QLineEdit;
    QLabel *timerIntervalLabel = new QLabel("Интервал (милисекунды):");
    QLineEdit *timerInterval = new QLineEdit;

    // Обработчик события для кнопки "Обзор" входного файла
    QObject::connect(inputFileButton, &QPushButton::clicked, [&] {
        QString inputFile = QFileDialog::getOpenFileName(nullptr, "Выберите входной файл", QString(), "Text Files (*.txt);;Binary Files (*.bin)");
        inputFileLineEdit->setText(inputFile);
    });

    // Обработчик события для кнопки "Обзор" выходного файла
    QObject::connect(outputFileButton, &QPushButton::clicked, [&] {
        QString outputFile = QFileDialog::getSaveFileName(nullptr, "Выберите выходной файл", QString(), "Text Files (*.txt);;Binary Files (*.bin)");
        outputFileLineEdit->setText(outputFile);
    });
    // Обработчик события для кнопки "Зашифровать"
    QObject::connect(encryptButton, &QPushButton::clicked, [&] {
        QString inputFile = inputFileLineEdit->text();
        QString outputFileName = outputFileLineEdit->text();
        QString fileExtension = QFileInfo(inputFile).suffix().toLower();
        QByteArray xorKey = QByteArray::fromHex(keyLineEdit->text().toUtf8());

        if (xorKey.size() != 8) {
            QMessageBox::critical(nullptr, "Ошибка", "Ключ должен содержать 8 байтов.");
            return;
        }

        if (inputFile.isEmpty() || outputFileName.isEmpty()) {
            QMessageBox::critical(nullptr, "Ошибка", "Выберите входной и выходной файлы.");
            return;
        }

        if (fileExtension != "txt" && fileExtension != "bin") {
            QMessageBox::critical(nullptr, "Ошибка", "Неподдерживаемое расширение файла. Поддерживаются только txt и bin.");
            return;
        }

        if (QFile::exists(outputFileName)) {
            QMessageBox msgBox;
            msgBox.setText("Файл с именем '" + outputFileName + "' уже существует.");
            msgBox.setInformativeText("Выберите действие:");
            QPushButton *edit=msgBox.addButton("Перезаписать", QMessageBox::YesRole);
            QPushButton *exit=msgBox.addButton("Отмена", QMessageBox::RejectRole);
            QPushButton *rename=msgBox.addButton( "Добавить счетчик к файлу", QMessageBox::NoRole);
            int choice ;
            choice = msgBox.exec();
            if(msgBox.clickedButton()==exit){
                return;
            }
            else if(msgBox.clickedButton()==edit){
                QMessageBox::information(nullptr, "Выполняется", "Операция  XOR с ключом и перезапись файла  начата для файла ." + fileExtension);
                QFile::remove(outputFileName);
            }
            else if(msgBox.clickedButton()==rename){
                QMessageBox::information(nullptr, "Выполняется", "Операция  XOR с ключом и перезапись с счетчиком  начата для файла ." + fileExtension);
                int counter = 1;
                QString baseName = QFileInfo(outputFileName).baseName();
                QString extension = QFileInfo(outputFileName).suffix();
                do {
                    outputFileName = baseName + "_" + QString::number(counter) + "." + extension;
                    counter++;
                } while (QFile::exists(outputFileName));
            }
        }

        if (inputFile == outputFileName) {
            QMessageBox::critical(nullptr, "Ошибка", "Входной и выходной файлы совпадают. Выберите другой выходной файл.");
            return;
        }

        QFile input(inputFile);
        if (!input.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(nullptr, "Ошибка", "Не удается открыть входной файл.");
            return;
        }

        QFile output(outputFileName);
        if (!output.open(QIODevice::WriteOnly)) {
            QMessageBox::critical(nullptr, "Ошибка", "Не удается открыть выходной файл.");
            return;
        }
        if(timerRunRadioButton->isChecked()){
                while (!input.atEnd()) {
                QMessageBox::critical(nullptr, "Ошибка", "0.");
                QByteArray data = input.read(4096);
                xorWithKey(data, xorKey);
                output.write(data);
                }
                input.close();
                output.close();
            int time=timerIntervalLineEdit->text().toInt();
                QTimer timer;
                 timer.start(time);
                while(timer.remainingTime()>0){
                 output.open(QIODevice::ReadOnly);
                     while (!output.atEnd()) {
                        QByteArray data_t=output.read(4096);
                        output.close();
                         xorWithKey(data_t, xorKey);
                        output.open(QIODevice::WriteOnly);
                         output.write(data_t);
                    }
                    output.close();
                }

        }
        else if(singleRunRadioButton->isChecked()){
        while (!input.atEnd()) {
            QByteArray data = input.read(4096);
            xorWithKey(data, xorKey);
            output.write(data);
        }
        input.close();
        output.close();
        }

        QMessageBox::information(nullptr, "Успех", "Операция XOR с ключом завершена для файла ." + fileExtension);
    });

    // Создаем макет для размещения виджетов
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(inputLabel);
    layout->addWidget(inputFileLineEdit);
    layout->addWidget(inputFileButton);
    layout->addWidget(outputLabel);
    layout->addWidget(outputFileLineEdit);
    layout->addWidget(outputFileButton);
    layout->addWidget(keyLabel);
    layout->addWidget(keyLineEdit);
    layout->addWidget(encryptButton);
    layout->addWidget(singleRunRadioButton);
    layout->addWidget(timerRunRadioButton);
    layout->addWidget(timerIntervalLabel);
    layout->addWidget(timerIntervalLineEdit);

    // Создаем центральный виджет и устанавливаем макет
    QWidget *centralWidget = new QWidget;
    centralWidget->setLayout(layout);
    window.setCentralWidget(centralWidget);

    window.show();

    return app.exec();
}
