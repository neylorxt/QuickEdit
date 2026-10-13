#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QDebug>
#include <QMessageBox>
#include <QTextEdit>
#include "CustomFileSystemModel.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // The first number is for the widget on the left ( TreeView ), and the second is for the one on the right ( TabWidget ).
    ui->splitter->setSizes({200, 600});

    // Choose a folder
    connect(ui->actionOpen_Folder, &QAction::triggered, this, [=](){

        folderPath = QFileDialog::getExistingDirectory(
            this,
            tr("Choose a folder"),
            "",
            QFileDialog::ShowDirsOnly
        );


        // Check whether the user has canceled
        if (!folderPath.isEmpty()) {

            // 1. Cleanup: If the view already had a model, retrieve its pointer to delete it
            if (ui->Hierarchy->model()) {
                QAbstractItemModel* oldModel = ui->Hierarchy->model();
                ui->Hierarchy->setModel(nullptr); // Detach the old model from the view
                delete oldModel;                  // We free the memory properly
            }

            // 2. Create the new model and assign it a parent (this)
            CustomFileSystemModel *hierarchyModel =
                new CustomFileSystemModel(this);

            hierarchyModel->setRootPath(folderPath);

            QString name = QFileInfo(folderPath).fileName();
            hierarchyModel->setFolderName(name);

            // 3. View Configuration
            ui->Hierarchy->setModel(hierarchyModel);
            ui->Hierarchy->setRootIndex(hierarchyModel->index(folderPath));

            // Hide the Size (2) and Date/Type (3) columns
            ui->Hierarchy->hideColumn(1); // Optional: Also hides the “Type” column if necessary
            ui->Hierarchy->hideColumn(2); // “Size” mask
            ui->Hierarchy->hideColumn(3); // “Last Modified” field

            // 4. Header Management
            ui->Hierarchy->setHeaderHidden(false);


        }


    } );

    // We connect the “clicked” signal from the Tree View to our custom slot
    connect(ui->Hierarchy, &QTreeView::clicked, this, &MainWindow::onItemClicked);

    // Save
    connect( ui->actionSave, &QAction::triggered, this, [=](){

        // Get the widget for the tab currently displayed on the screen
        QWidget *currentWidget = ui->tabs->currentWidget();

        if (!currentWidget) return; // Aucun onglet ouvert

        // Convert it to a QTextEdit
        QTextEdit *currentEditor = qobject_cast<QTextEdit*>(currentWidget);

        if (currentEditor) {
            // Retrieve the path to the file we hid in 'whatsThis'
            QString filePath = currentEditor->whatsThis();

            // Save
            QFile file(filePath);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                out.setEncoding(QStringConverter::Encoding::Utf8);
                out << currentEditor->toPlainText();
                file.close();
                statusBar()->showMessage("Fichier enregistré : " + QFileInfo(filePath).fileName(), 2000);
            }
        }

    } );

    // Create a new file
    connect( ui->actionNew_File, &QAction::triggered, this, [=](){

        if(folderPath.isEmpty()){
            return;
        }

        bool ok;
        QString fileName = QInputDialog::getText(
            this,
            tr("New File"),           // Popup Title
            tr("File name :"),        // Placeholder text above the field
            QLineEdit::Normal,        // Text Display Mode
            "new_file.txt",           // Default text in the field
            &ok                       // Becomes ‘true’ if the user clicks OK
        );

        if (ok && !fileName.isEmpty()) {

            // Sécurité : On assemble le chemin complet
            QString fullPath = folderPath + "/" + fileName;

            // Create the file on the disk
            QFile file(fullPath);
            if (file.open(QIODevice::WriteOnly)) {
                file.close(); // We close it right away to keep it empty

                // A short confirmation message in the status bar
                statusBar()->showMessage("File created : " + fileName, 2000);
            } else {
                QMessageBox::critical(this, "Error", "Unable to create the file.\nCheck the characters or permissions.");
            }
        }
    });

    // Create New Folder
    connect( ui->actionNew_Folder, &QAction::triggered, this, [=](){

        if (folderPath.isEmpty()) {
            return;
        }

        bool ok;
        QString folderName = QInputDialog::getText(
            this,
            tr("New Folder"),           // Popup Title
            tr("Folder name :"),        // Texte au-dessus du champ
            QLineEdit::Normal,          // Mode d'affichage
            "new_folder",               // Texte par défaut
            &ok                         // Devient 'true' si l'utilisateur clique sur OK
            );

        if (ok && !folderName.isEmpty()) {

            // Sécurité : On assemble le chemin complet du nouveau dossier
            QString fullPath = folderPath + "/" + folderName;

            // On utilise QDir pour manipuler les dossiers
            QDir dir;

            // mkpath crée le dossier et tous les sous-dossiers parents si nécessaire
            if (dir.mkpath(fullPath)) {

                // Message de confirmation dans la barre de statut
                statusBar()->showMessage("Folder created : " + folderName, 2000);

            } else {
                QMessageBox::critical(this, "Error", "Unable to create the folder.\nCheck permissions or special characters.");
            }
        }

    } );

    // 1. Be sure to click the close buttons (the small “x” icons)
    ui->tabs->setTabsClosable(true);

    connect(ui->tabs, &QTabWidget::tabCloseRequested, this, [=](int index) {
        QWidget *w = ui->tabs->widget(index);
        ui->tabs->removeTab(index); // Hides the tab from view
        delete w;                  // Cleanly removes the QTextEdit from memory
    });


}

MainWindow::~MainWindow()
{

    // connect(ui->tabs, &QTabWidget::tabCloseRequested, this, [=](int index) {
    //     QWidget *w = ui->tabs->widget(index);
    //     ui->tabs->removeTab(index); // Hides the tab from view
    //     delete w;                  // Cleanly removes the QTextEdit from memory
    // });

    delete ui;

}

void MainWindow::onItemClicked(const QModelIndex &index)
{
    // Retrieve the model associated with the view
    // We need to “cast” it to QFileSystemModel because ui->Hierarchy->model() returns a generic model
    QFileSystemModel *model = qobject_cast<QFileSystemModel*>(ui->Hierarchy->model());
    bool isFolder = model->isDir(index);  // Is this a folder or a file?

    if (model && !isFolder ) {
        // Retrieve information from the clicked file or folder
        QString currentSelectedFilePath = model->filePath(index);       // Full path (e.g., C:/Folder/test.txt)
        QString fileName = model->fileName(index);       // Just the filename (e.g., test.txt)


        // STEP 1: Check whether the file is already open in a tab
        for (int i = 0; i < ui->tabs->count(); ++i) {

            // We use the ‘whatsThis’ or ‘objectName’ property to store the full file path
            if (ui->tabs->widget(i)->whatsThis() == currentSelectedFilePath) {
                ui->tabs->setCurrentIndex(i); // The file is already open; let's display it
                return;
            }
        }

        // STEP 2: Read the contents of the file
        QFile file(currentSelectedFilePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

        QTextStream in(&file);
        in.setEncoding(QStringConverter::Encoding::Utf8);
        QString fileContent = in.readAll();
        file.close();

        // STEP 3: Dynamically create the text component for the new tab
        QTextEdit *newEditor = new QTextEdit(this);
        newEditor->setText(fileContent);

        // TIP: We store the full path of the hidden file in the widget
        // This will let us know which file to save later!
        newEditor->setWhatsThis(currentSelectedFilePath);

        // STEP 4: Add the widget to the QTabWidget
        int tabIndex = ui->tabs->addTab(newEditor, fileName);

        // Activer (mettre au premier plan) le nouvel onglet créé
        ui->tabs->setCurrentIndex(tabIndex);

    }
}
