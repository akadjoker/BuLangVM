const vscode = require('vscode');

function activate(context) {

    // Regista o formatador
    const disposable = vscode.languages.registerDocumentFormattingEditProvider('bulang', {
        provideDocumentFormattingEdits(document) {
            const edits = [];
            const lineCount = document.lineCount;

            for (let i = 0; i < lineCount; i++) {
                const line = document.lineAt(i);
                const text = line.text;

                // Regex: Captura indentação (grupo 1) e código (grupo 2)
                // Procura linhas que acabam em "{" e têm algo escrito antes
                const regex = /^(\s*)(.*[^\s])\s*\{\s*$/;
                const match = text.match(regex);

                if (match) {
                    const indent = match[1]; // Espaços ou tabs
                    const code = match[2];   // O código (ex: "process teste()")

                    // Constrói o novo texto com a quebra de linha estilo Allman
                    const newText = indent + code + "\n" + indent + "{";

                    // Cria a edição
                    const edit = vscode.TextEdit.replace(line.range, newText);
                    edits.push(edit);
                }
            }
            return edits;
        }
    });

    context.subscriptions.push(disposable);
}

function deactivate() {}

module.exports = {
    activate,
    deactivate
};
