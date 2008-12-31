/*
 *  RibRender.cpp
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/31.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#include <stdio.h>
#include "RI_Parser.h"
#include "RI_Machine.h"
#include "DUtils.h"

#include "RibRender.h"

#include <GLUT/glut.h>

//===============================================================
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glCallList( 1 );

    glutSwapBuffers();
}

//===============================================================
void reshape(int width, int height)
{
    glViewport(0, 0, width, height);

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D(0, width, height, 0);
	
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	//glTranslatef( -0.375f, -0.375f, 0 );

	//gApp.SetSize( width, height );
}

//===============================================================
static void passiveMotion( int x, int y )
{
	//gApp.SetCursorPos( x, y );
	glutPostRedisplay();
}

//===============================================================
void idle(void)
{
    glutPostRedisplay();
}

//===============================================================
int main(int argc, char** argv)
{
	if ( argc != 2 )
	{
		printf( "Invalid param count. Quitting !\n" );
		return -1;
	}

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    
    glutCreateWindow( "RibRender" );

    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
	glutPassiveMotionFunc( passiveMotion );

    glutIdleFunc(idle);
    
	void	*pData;
	size_t	dataSize;

	if NOT( DUT::GrabFile( argv[1], pData, dataSize ) )
	{
		printf( "Could not open the file in input. Quitting !\n" );
		return -1;
	}

	RI::Parser		parser;
	RI::Machine		machine;
	
	glNewList( 1, GL_COMPILE );

	for (size_t i=0; i <= dataSize; ++i)
	{
		if ( i == dataSize )
			parser.AddChar( 0 );
		else
			parser.AddChar( ((char *)pData)[i] );

		if ( parser.HasNewCommand() )
		{
			DStr			cmdName;
			RI::ParamList	cmdParams;

			parser.FlushNewCommand( &cmdName, &cmdParams );

			printf( "CMD %s ", cmdName.c_str() );
			
			if ( cmdParams.size() )
				printf( "(%i params)", cmdParams.size() );

			puts( "" );

			machine.AddCommand( cmdName, cmdParams );
		}
		
	}
	
	glEndList();

	glutMainLoop();

    return EXIT_SUCCESS;
}
