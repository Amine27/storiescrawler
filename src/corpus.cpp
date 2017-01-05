/***************************************************************************
 *   Copyright (C) 2010 by Amine Roukh           <amineroukh@gmail.com>    *
 *   Copyright (C) 2010 by Abdelkadir Sadouki    <sadouki_aek@hotmail.fr>  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include "corpus.h"

Corpus::Corpus(QObject *parent)
        : QObject(parent)
{
}

QStringList Corpus::extractWords(QString target)
{
    numberOfWords = 0;
    numberOfChars = 0;

    //qDebug() << target;

    // Split target string into words saparated by space
    QStringList wordIterator = target.split(" ");
    QStringList words;

    for (int i=0; i < wordIterator.size(); i++)
    {
        QString word = wordIterator.at(i);

        if(word.size() > 0 && word.at(0).isLetterOrNumber())
        {
            // Remove words punctuation
            QString wordWithoutPunct;
            int j=0;
            while( j < word.size())
            {
                if(!word.at(j).isPunct())
                    wordWithoutPunct.append(word.at(j));

                j++;
            }

            words.append(wordWithoutPunct);
            //qDebug() << wordWithoutPunct;

            numberOfChars += wordWithoutPunct.size();
        }
    }

    numberOfWords = words.size();
    //qDebug() << "number of word: " << numberOfWords;
    //qDebug() << "Number of char: " << numberOfChars;

    return words;
}

int Corpus::getCharsCount()
{
    return numberOfChars;
}

int Corpus::getWordsCount()
{
    return numberOfWords;
}
