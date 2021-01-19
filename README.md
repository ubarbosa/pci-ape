# pci-ape
Show pci buses connections (as lspci, but graphically or color-text based)


pciape [-t]

Sem argumentos, o programa tenta mostrar graficamente como os componentes se conectam
aos buses.

Options:

    -t : Imprime as informações no próprio terminal. Parecido com o lspci, mas com algumas features.
    
Para compilar, basta executar

    make

E para rodar, depois de compilado

    ./pciape

Se preferir, no diretório bin já existe uma versão compilada para PC amd64. Para executá-lo direto, sem 
precisar compilar

    ./bin/pciape
