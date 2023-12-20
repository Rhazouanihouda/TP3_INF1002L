/**
 * \file block.cpp
 * \brief Implémentation d'un bloc.
 * \author ?
 * \version 0.1
 * \date  2023
 *
 *  Travail pratique numéro 3
 *
 */

#include "block.h"
// vous pouvez inclure d'autres librairies si c'est nécessaire

namespace TP3
{
	Block::Block()
	{
	}

	Block::Block(size_t td)
	{
		m_type_donnees = td;
	}

	Block::~Block()
	{
		if (m_inode != nullptr)
		{
			delete m_inode;
			m_inode = nullptr;
		}

		for (int i = 0; i < m_dirEntry.size(); i++)
		{
			if (m_dirEntry[i] != nullptr)
			{
				delete m_dirEntry[i];
				m_dirEntry[i] = nullptr;
			}
		}
	}

	// Ajouter votre code ici !
}