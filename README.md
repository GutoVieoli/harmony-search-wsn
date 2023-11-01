# 🎶 Harmony Search para Otimização de Posicionamento de Sensores em Redes Sem Fio 📡

## 🌟 Introdução

Redes de Sensores Sem Fio (WSN - Wireless Sensors Network) têm se mostrado fundamentais em diversas áreas, incluindo monitoramento ambiental 🌲, agricultura 🚜 e detecção de ameaças 🚨. Essas redes são formadas por sensores capazes de coletar e transmitir informações em tempo real, desempenhando um papel crucial em várias aplicações. O desafio? **Otimizar a cobertura desses sensores**, utilizando o menor número possível de dispositivos.

<div align="center">
  <img height="250em" src="https://upload.wikimedia.org/wikipedia/commons/thumb/0/0b/WSN2.svg/640px-WSN2.svg.png" alt="Wireless Sensor Network"/>
</div>


Neste projeto, nosso foco está na otimização do posicionamento de sensores em uma WSN. Estamos em busca da máxima cobertura da rede, garantindo eficiência na instalação dos sensores, minimização de custos e conectividade entre eles.

## 👨‍🔬 Autores
- Augusto Vieira de Oliveira
- Iago Augusto de Carvalho

## 🔍 Motivação e Problema

Estamos enfrentando o desafio do **Problema de Maximizar a Cobertura de uma WSN**. Queremos encontrar a disposição ótima dos sensores em um ambiente bidimensional para maximizar a cobertura e, ao mesmo tempo, minimizar a quantidade de sensores necessários. Devido à complexidade exponencial desse problema, recorremos a heurísticas e metaheurísticas para encontrar soluções.

## 📜 Trabalhos Relacionados

Este trabalho se inspira em estudos prévios importantes. As contribuições de **Alia & Al Ajouri (2016)** foram cruciais, com a introdução da metaheurística de Busca Harmônica aplicada a redes de sensores sem fio. Além disso, **Bajaj Manju (2014)** estendeu o problema para incluir a maximização da cobertura com foco em eficiência energética.

## 🛠 O Algoritmo

Desenvolvemos uma versão aprimorada do algoritmo de Busca Harmônica, especialmente adaptada para as necessidades das WSNs. Nosso algoritmo visa otimizar a instalação eficiente dos sensores, reduzir custos e garantir uma conectividade robusta entre os dispositivos. Utilizamos a **memória harmônica** para armazenar soluções e aplicamos melhorias locais e aleatórias na geração de novas soluções.

Para avaliar a qualidade das soluções, consideramos o número de sensores, a cobertura da rede e a distância entre os sensores. As soluções são representadas por dois vetores: um para as posições dos sensores e outro, binário, indicando a ativação dos sensores.

## 📖 Como Usar

Para começar a usar nosso algoritmo, siga estas etapas:

1. **Clone o Repositório**: `git clone https://example.com/your_repository`
2. **Instale as Dependências**: Certifique-se de ter todas as bibliotecas e ferramentas necessárias instaladas.
3. **Configure o Algoritmo**: Ajuste os parâmetros conforme sua necessidade.
4. **Execute**: Rode o algoritmo e observe a mágica acontecer! 🚀
5. **Comando para compilar**: `g++ main.cpp funcs.cpp -o main`

## 📞 Contato

Para dúvidas, sugestões ou para discutir sobre o projeto:

- 📧 **Email**: augustovieira@sou.unifal-mg.edu.br
- 💼 **LinkedIn**: [Perfil de Augusto](https://www.linkedin.com/in/augustovieoli)

