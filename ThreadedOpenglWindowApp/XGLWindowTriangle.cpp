#include "XGLWindowTriangle.h"
#include <QColor>
#include <QPainter>

#include <QFile>
#include <QTimer>
#include <QKeyEvent>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

/////////////////////////////////////////////////////////////////
inline static constexpr float _logicalUnit = (float)(0.5f);
inline static constexpr float _logicalHeight = 2.0f * _logicalUnit;

#define SML_SCALE(x) ((x)*(_logicalUnit))


/////////////////////////////////////////////////////////////////
static constexpr float DISTANCE_TRIANGLE = -6.0f;
static constexpr float DISTANCE_POINT = -4.5f;

static GLfloat oglpos[] =
{
	//    SML_SCALE(1.0f),    SML_SCALE(-1.0f),   SML_SCALE(DISTANCE_TRIANGLE), 1.0f,
	//    SML_SCALE(0.0f),    SML_SCALE(1.0f),    SML_SCALE(DISTANCE_TRIANGLE), 1.0f,
	//    SML_SCALE(-1.0f),   SML_SCALE(-1.0f),   SML_SCALE(DISTANCE_TRIANGLE), 1.0f,
	//    SML_SCALE(0.0f),    SML_SCALE(0.0f),    SML_SCALE(DISTANCE_POINT), 1.0f,

		SML_SCALE(1.0f),    SML_SCALE(-1.0f),   SML_SCALE(0.0f), 1.0f,
		SML_SCALE(0.0f),    SML_SCALE(1.0f),    SML_SCALE(0.0f), 1.0f,
		SML_SCALE(-1.0f),   SML_SCALE(-1.0f),   SML_SCALE(0.0f), 1.0f,
		SML_SCALE(0.0f),    SML_SCALE(0.0f),    SML_SCALE(-1.5f), 1.0f,
};

static GLfloat oglcolor[] =
{
	1.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
};

static GLfloat texCoords[] =
{
	1.0f, 0.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
};



static GLuint oglindics[] = {
	0, 2, 1,
	0, 1, 3,
	1, 2, 3,
	0, 3, 2,
};


//static GLfloat oglLinepos[] =
//{
//    SML_SCALE(-10.0),    SML_SCALE(0.0f),   SML_SCALE(0.0f), 1.0f,
//    SML_SCALE(10.0),    SML_SCALE(0.0f),   SML_SCALE(0.0f), 1.0f,

//    SML_SCALE(0.0),    SML_SCALE(-10.0f),   SML_SCALE(0.0f), 1.0f,
//    SML_SCALE(0.0),    SML_SCALE(10.0f),   SML_SCALE(0.0f), 1.0f,

//    SML_SCALE(0.0),    SML_SCALE(0.0f),   SML_SCALE(-10.0f), 1.0f,
//    SML_SCALE(0.0),    SML_SCALE(0.0f),   SML_SCALE(10.0f), 1.0f,

//};

//static GLfloat oglLinecolor[] =
//{
//    0.0f, 0.0f, 0.0f, 1.0f,
//    1.0f, 1.0f, 1.0f, 1.0f,

//    0.0f, 0.0f, 0.0f, 1.0f,
//    1.0f, 1.0f, 1.0f, 1.0f,


//    0.0f, 0.0f, 0.0f, 1.0f,
//    1.0f, 1.0f, 1.0f, 1.0f,

//};

//static GLuint oglLineindics[] = {
//    0, 1,
//    2, 3,
//    4, 5,
//};


void XGLWindowTriangle::CreateProgram(const GLchar* const vertSource, const GLchar* const  fragSource)
{
	/////////////////////////////////////////////////////////////////
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertShader, 1, &vertSource, NULL); // vertex_shader_source is a GLchar* containing glsl shader source code
	glCompileShader(vertShader);

	/////////////////////////////////////////////////////////////////
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &fragSource, NULL); // vertex_shader_source is a GLchar* containing glsl shader source code
	glCompileShader(fragShader);

	/////////////////////////////////////////////////////////////////
	_programId = glCreateProgram();

	glAttachShader(_programId, vertShader);
	glAttachShader(_programId, fragShader);

	glLinkProgram(_programId);

	/////////////////////////////////////////////////////////////////
	glDeleteShader(vertShader);
	vertShader = 0;

	glDeleteShader(fragShader);
	fragShader = 0;
}


void XGLWindowTriangle::GLInitialize()
{
	/////////////////////////////////////////////////////////////////
	//ResetEye();
	_axisEye.Reset();
	_axisModel.Reset();
	_axisModel.SetOrigin(glm::vec3(0.0f, 0.0f, SML_SCALE(DISTANCE_POINT)));

	/////////////////////////////////////////////////////////////////
	//initializeOpenGLFunctions();

	//connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &MyOglWidget::on_aboutToBeDestroyed);

	//glDebugMessageCallback(&MyOglWidget::DEBUGPROC, this);

	/////////////////////////////////////////////////////////////////
	QFile filevert{ ":/shaders/vert.vert" };
	filevert.open(QFile::ReadOnly);
	QByteArray vertBuffer = filevert.readAll();
	filevert.close();

	QFile filefrag{ ":/shaders/frag.frag" };
	filefrag.open(QFile::ReadOnly);
	QByteArray fragBuffer = filefrag.readAll();
	filefrag.close();

	CreateProgram(vertBuffer.data(), fragBuffer.data());

	/////////////////////////////////////////////////////////////////
	glCreateBuffers(1, &_vboPos);
	glCreateBuffers(1, &_vboColor);
	glCreateBuffers(1, &_vboTextCoord);
	glCreateBuffers(1, &_vboElemet);


	glNamedBufferData(_vboPos, sizeof(oglpos), oglpos, GL_STATIC_DRAW);
	glNamedBufferData(_vboColor, sizeof(oglcolor), oglcolor, GL_STATIC_DRAW);
	glNamedBufferData(_vboTextCoord, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
	glNamedBufferData(_vboElemet, sizeof(oglindics), oglindics, GL_STATIC_DRAW);



	//QImage imageTemp{QString::fromUtf8(":/image/side5.png")};
	QImage imageTemp{ QString::fromUtf8(":/image/tex.jpg") };
	//QImage image = imageTemp.convertToFormat(QImage::Format_RGB888);
	QImage image = imageTemp.convertToFormat(QImage::Format_RGBA8888);
	QImage::Format format = image.format();
	bool hasalpha = image.hasAlphaChannel();
	image.mirror();


#if 0
	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_2D, _texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_TEXTURE_MAG_FILTER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_TEXTURE_MAG_FILTER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(
		GL_TEXTURE_2D,//                GLenum target,
		0,//    GLint level,
		GL_RGBA,//    GLint internalFormat,
		image.width(),//    GLsizei width,
		image.height(),//    GLsizei height,
		0,//    GLint border,
		GL_RGBA,//    GLenum format,
		GL_UNSIGNED_BYTE,//    GLenum type,
		image.constBits()//    const GLvoid * data
	);
	glGenerateMipmap(GL_TEXTURE_2D);


	glBindTexture(GL_TEXTURE_2D, 0);
#else

	glCreateTextures(GL_TEXTURE_2D, 1, &_texture); //to cleanup

	glTextureParameteri(
		_texture,//                GLuint texture,
		GL_TEXTURE_MAG_FILTER,//    GLenum pname,
		GL_LINEAR//    GLfloat param
	);

	glTextureParameteri(
		_texture,//                GLuint texture,
		GL_TEXTURE_MIN_FILTER,//    GLenum pname,
		GL_LINEAR//    GLfloat param
	);

	glTextureParameteri(
		_texture,//                GLuint texture,
		GL_TEXTURE_WRAP_S,//    GLenum pname,
		GL_REPEAT//    GLfloat param
	);


	glTextureParameteri(
		_texture,//                GLuint texture,
		GL_TEXTURE_WRAP_T,//    GLenum pname,
		GL_REPEAT//    GLfloat param
	);

	glTextureStorage2D(
		_texture,//                    GLuint texture,
		8,//        GLsizei levels,
		GL_RGBA8,//        GLenum internalformat,
		image.width(), //        GLsizei width,
		image.height()//        GLsizei height
	);

	glTextureSubImage2D(
		_texture,//                GLuint texture,
		0,//    GLint level,
		0,//    GLint xoffset,
		0,//    GLint yoffset,
		image.width(),//    GLsizei width,
		image.height(),//    GLsizei height,
		GL_RGBA,//    GLenum format,
		GL_UNSIGNED_BYTE,//    GLenum type,
		image.constBits()//    const void *pixels
	);

	glGenerateTextureMipmap(_texture);

#endif



	/////////////////////////////////////////////////////////////////
	//glCreateBuffers(1, &_vboPosLine);
//    glCreateBuffers(1, &_vboColorLine);
//    glCreateBuffers(1, &_vboElemetLine);

//    glNamedBufferData(_vboPosLine, sizeof(oglLinepos), oglLinepos,  GL_STATIC_DRAW);
//    glNamedBufferData(_vboColorLine, sizeof(oglLinecolor), oglLinecolor, GL_STATIC_DRAW);
//    glNamedBufferData(_vboElemetLine, sizeof(oglLineindics), oglLineindics, GL_STATIC_DRAW);


	/////////////////////////////////////////////////////////////////
	glCreateVertexArrays(1, &_vao);

	glVertexArrayAttribBinding(_vao, posLocation, posLocation);
	glVertexArrayAttribBinding(_vao, colorLocation, colorLocation);
	glVertexArrayAttribBinding(_vao, texCoordLocation, texCoordLocation);

	glVertexArrayAttribFormat(
		_vao,//GLuint vaobj,
		posLocation,//GLuint attribindex,
		4,//GLuint size,
		GL_FLOAT,//GLenum type,
		false,//GLboolean normalized,
		0//GLuint relativeoffset
	);

	glVertexArrayAttribFormat(
		_vao,//GLuint vaobj,
		colorLocation,//GLuint attribindex,
		4,//GLuint size,
		GL_FLOAT,//GLenum type,
		false,//GLboolean normalized,
		0//GLuint relativeoffset
	);


	glVertexArrayAttribFormat(
		_vao,//GLuint vaobj,
		texCoordLocation,//GLuint attribindex,
		2,//GLuint size,
		GL_FLOAT,//GLenum type,
		false,//GLboolean normalized,
		0//GLuint relativeoffset
	);

	/////////////////////////////////////////////////////////////////
	glVertexArrayVertexBuffer(
		_vao,//GLuint vaobj,
		posLocation,//GLuint bindingindex,
		_vboPos,//GLuint buffer,
		0,//GLuintptr offset,
		sizeof(GLfloat) * 4//,//GLsizei stride
	);

	glVertexArrayVertexBuffer(
		_vao,//GLuint vaobj,
		colorLocation,//GLuint bindingindex,
		_vboColor,//GLuint buffer,
		0,//GLuintptr offset,
		sizeof(GLfloat) * 4//,//GLsizei stride
	);

	glVertexArrayVertexBuffer(
		_vao,//GLuint vaobj,
		texCoordLocation,//GLuint bindingindex,
		_vboTextCoord,//GLuint buffer,
		0,//GLuintptr offset,
		sizeof(GLfloat) * 2//,//GLsizei stride
	);

	glVertexArrayElementBuffer(_vao, _vboElemet);

	glEnableVertexArrayAttrib(_vao, posLocation);
	glEnableVertexArrayAttrib(_vao, colorLocation);
	glEnableVertexArrayAttrib(_vao, texCoordLocation);



	/////////////////////////////////////////////////////////////////
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);
}


void XGLWindowTriangle::on_timeout()
{


	static constexpr float angle_delta = 1.0f;
	//static constexpr float angle_delta = 0.0f; //no rotation
	float radians = glm::radians(angle_delta);
	_axisModel.Rotate(radians, glm::vec3(1.0f, 0.0f, 0.0f))
		.Rotate(radians, glm::vec3(0.0f, 1.0f, 0.0f))
		.Rotate(radians, glm::vec3(0.0f, 0.0f, 1.0f));
	static constexpr float offset_delta = SML_SCALE(0.1f);
	//    if(_dirInc)
	//    {
	//        _offsetZ += offset_delta;
	//        _axisModel.Translate(glm::vec3(0.0f, 0.0f, offset_delta));
	//        if(_offsetZ >= -SML_SCALE(DISTANCE_TRIANGLE))
	//        {
	//            _dirInc = false;
	//        }
	//    }
	//    else
	//    {
	//        _offsetZ -= offset_delta;
	//        _axisModel.Translate(glm::vec3(0.0f, 0.0f, -offset_delta));
	//        if(_offsetZ <= SML_SCALE(DISTANCE_TRIANGLE))
	//        {
	//            _dirInc = true;
	//        }
	//    }


	//if (this->isVisible() && !this->isHidden())
	//{
	//	this->update();
	//}
}


void XGLWindowTriangle::GLResize(const QSize& size, const QSize& oldSize)
{
	int w = size.width();
	int h = size.height();
	glViewport(0, 0, size.width(), size.height());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	QColor bgcolor = Qt::darkCyan;
	glClearColor(bgcolor.redF(), bgcolor.greenF(), bgcolor.blueF(), 1.0f);

	/////////////////////////////////////////////////////////////////
	float halfWidth = _logicalUnit * w / h;

#if true

	_frustum = glm::frustum<float>(-halfWidth, halfWidth,
		-_logicalUnit, _logicalUnit,
		_logicalHeight, 512.0f * _logicalHeight);

#else

	_frustum = SmartLib::AxisSystem<float>::Ortho(-halfWidth, halfWidth,
		-_logicalUnit, _logicalUnit,
		_logicalHeight, 512.0f * _logicalHeight);

#endif

}

void XGLWindowTriangle::GLPaint(QPaintDevice* paintDev)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	QColor bgcolor = Qt::darkCyan;
	glClearColor(bgcolor.redF(), bgcolor.greenF(), bgcolor.blueF(), 1.0f);


	QPainter painter{ paintDev };

	painter.setPen(Qt::white);
	painter.setFont(QFont("Arial", 30));
	painter.drawText(50, 50,  QString::number(++_counter));

	painter.end();

	/////////////////////////////////////////////////////////////////


	//    glm::mat4 view = glm::lookAt<float>(
	//                _eye,
	//                _eye + glm::vec3(_eyeAxis[2]), //lookinto -z
	//            glm::vec3(_eyeAxis[1])); //upper y

	glm::mat4 view = _axisEye.WorldToModelMat();


	/////////////////////////////////////////////////////////////////
	//glm::mat4 modelT{1.0f};
	//    glm::mat4 modelT = glm::translate(glm::mat4(1.0f),
	//                                      glm::vec3(SML_SCALE(glm::sin(radians))*2.0f,
	//                                                SML_SCALE(glm::cos(radians))*2.0f,
	//                                                SML_SCALE(glm::sin(2*radians))*0.0f));

	auto model = _axisModel.ModelToWorldMat();


	//glm::mat4 modelS{1.0f};
	//    glm::mat4 modelS = glm::scale(glm::mat4(1.0f),
	//            glm::vec3(glm::max(glm::cos(radians), 0.6f) ,
	//                      glm::max(glm::sin(radians), 0.6f) ,
	//                      glm::max(glm::cos(2*radians), 0.6f)));


	//glm::mat4  model = modelT * modelR * modelS;

	glm::mat4 mvp = _frustum * view * model;


	/////////////////////////////////////////////////////////////////
#if 0
	{
		int index = 0;
		auto vec0 = glm::vec4(oglpos[index], oglpos[index + 1], oglpos[index + 2], oglpos[index + 3]);
		index += 4;

		auto vec1 = glm::vec4(oglpos[index], oglpos[index + 1], oglpos[index + 2], oglpos[index + 3]);
		index += 4;

		auto vec2 = glm::vec4(oglpos[index], oglpos[index + 1], oglpos[index + 2], oglpos[index + 3]);
		index += 4;

		auto vec3 = glm::vec4(oglpos[index], oglpos[index + 1], oglpos[index + 2], oglpos[index + 3]);
		index += 4;

		auto v0 = mvp * vec0;
		auto v1 = mvp * vec1;
		auto v2 = mvp * vec2;
		auto v3 = mvp * vec3;

		auto str0 = glm::to_string(v0 / v0[3]);
		auto str1 = glm::to_string(v1 / v1[3]);
		auto str2 = glm::to_string(v2 / v2[3]);
		auto str3 = glm::to_string(v3 / v3[3]);

	}
#endif

	/////////////////////////////////////////////////////////////////
	glUseProgram(_programId);
	glBindVertexArray(_vao);

	//static constexpr int mvpLocation = 0;
	if (-1 == _mvpLocation)
	{
		//auto xxxposLocation = glGetAttribLocation(_programId, "pos"); //to xxx
		//auto xxxcolorLocation = glGetAttribLocation(_programId, "color"); //to xxx
		_mvpLocation = glGetUniformLocation(_programId, "mvp");
	}
	glProgramUniformMatrix4fv(_programId, _mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));

	if (-1 == _texCoordLocation)
	{
		_texCoordLocation = glGetUniformLocation(_programId, "tex");
	}
	glProgramUniform1i(_programId, _texCoordLocation, 0);

	//glActiveTexture(GL_TEXTURE0);
	glBindTextureUnit(0, _texture);


	glDrawElements(GL_TRIANGLES, sizeof(oglindics) / sizeof(oglindics[0]), GL_UNSIGNED_INT, 0);

	/////////////////////////////////////////////////////////////////
//    glVertexArrayVertexBuffer(
//                _vao,//GLuint vaobj,
//                posLocation,//GLuint bindingindex,
//                _vboPosLine,//GLuint buffer,
//                0,//GLuintptr offset,
//                sizeof(GLfloat)*4//,//GLsizei stride
//                );

//    glVertexArrayVertexBuffer(
//                _vao,//GLuint vaobj,
//                colorLocation,//GLuint bindingindex,
//                _vboColorLine,//GLuint buffer,
//                0,//GLuintptr offset,
//                sizeof(GLfloat)*4//,//GLsizei stride
//                );

//    glVertexArrayElementBuffer(_vao, _vboElemetLine);

//    glDrawElements(GL_LINES, sizeof(oglLineindics)/sizeof(oglLineindics[0]), GL_UNSIGNED_INT, 0);

	/////////////////////////////////////////////////////////////////
	glBindTextureUnit(0, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	/////////////////////////////////////////////////////////////////
	//context()->swapBuffers(context()->surface()); //no need to call swapBuffers mannually
	//update(); //animating
//    if(!_updateTimer->isActive())
//    {
//        _updateTimer->start(); //animating
//    }
}

void XGLWindowTriangle::GLFinalize()
{
	/////////////////////////////////////////////////////////////////
	if (_vboPos != -1)
	{
		glDeleteBuffers(1, &_vboPos);
		_vboPos = -1;
	}

	if (_vboColor != -1)
	{
		glDeleteBuffers(1, &_vboColor);
		_vboColor = -1;
	}

	if (_vboElemet != -1)
	{
		glDeleteBuffers(1, &_vboElemet);
		_vboElemet = -1;
	}

	if (_vboTextCoord != -1)
	{
		glDeleteBuffers(1, &_vboTextCoord);
		_vboTextCoord = -1;
	}

	if (_texture != -1)
	{
		glDeleteTextures(1, &_texture);
		_texture = -1;
	}


	/////////////////////////////////////////////////////////////////
//    if(_vboPosLine != -1)
//    {
//        glDeleteBuffers(1, &_vboPosLine);
//        _vboPosLine = -1;
//    }

//    if(_vboColorLine != -1)
//    {
//        glDeleteBuffers(1, &_vboColorLine);
//        _vboColorLine = -1;
//    }

//    if(_vboElemetLine != -1)
//    {
//        glDeleteBuffers(1, &_vboElemetLine);
//        _vboElemetLine = -1;
//    }


	/////////////////////////////////////////////////////////////////
	if (_vao != -1)
	{
		glDeleteVertexArrays(1, &_vao);
		_vao = -1;
	}
}

void XGLWindowTriangle::keyPressEvent(QKeyEvent* ev)
{
	//int key = ev->key();
	//switch (key)
	//{
	//case Qt::Key_Space:
	//	_isAnimating = !_isAnimating;
	//	SetAnimating(_isAnimating);
	//	break;

	//default:
	//	XQTBase::keyPressEvent(ev);
	//	break;
	//}


	/////////////////////////////////////////////////////////////////
	static const glm::vec3 AxisX{ 1.0f, 0.0f, 0.0f };
	static const glm::vec3 AxisY{ 0.0f, 1.0f, 0.0f };
	static const glm::vec3 AxisZ{ 0.0f, 0.0f, 1.0f };


	static constexpr float ratio{ 0.1f };
	static constexpr float angleDelta{ 2.0f };

	/////////////////////////////////////////////////////////////////
	switch (ev->key())
	{

	case Qt::Key_Space:
	{
		_isAnimating = !_isAnimating;
		SetAnimating(_isAnimating);
		//ResetEye();
		_axisEye.Reset();
		_axisModel.Reset();
		_axisModel.SetOrigin(glm::vec3(0.0f, 0.0f, SML_SCALE(DISTANCE_POINT)));
		requestUpdate();
	}
	break;

	case Qt::Key_W:
	{
		_axisEye.Translate(glm::vec3{ 0.0f, 0.0f, SML_SCALE(-ratio) });
	}
	break;

	case Qt::Key_S:
	{
		_axisEye.Translate(glm::vec3{ 0.0f, 0.0f, SML_SCALE(+ratio) });
	}
	break;

	case Qt::Key_A:
	{
		_axisEye.Translate(glm::vec3{ SML_SCALE(-ratio), 0.0f, 0.0f });
	}
	break;

	case Qt::Key_D:
	{
		_axisEye.Translate(glm::vec3{ SML_SCALE(+ratio), 0.0f, 0.0f });
	}
	break;

	case Qt::Key_Q:
	{
		_axisEye.Translate(glm::vec3{ 0.0f, SML_SCALE(-ratio), 0.0f });
	}
	break;

	case Qt::Key_E:
	{
		_axisEye.Translate(glm::vec3{ 0.0f, SML_SCALE(+ratio), 0.0f });
	}
	break;


	case Qt::Key_I:
	{
		_axisEye.Rotate(glm::radians(-angleDelta), glm::vec3{ 1.0f, 0.0f, 0.0f });
	}
	break;

	case Qt::Key_K:
	{
		_axisEye.Rotate(glm::radians(+angleDelta), glm::vec3{ 1.0f, 0.0f, 0.0f });
	}
	break;


	case Qt::Key_J:
	{
		_axisEye.Rotate(glm::radians(+angleDelta), glm::vec3{ 0.0f, 0.0f, 1.0f });
	}
	break;

	case Qt::Key_L:
	{
		_axisEye.Rotate(glm::radians(-angleDelta), glm::vec3{ 0.0f, 0.0f, 1.0f });
	}
	break;

	case Qt::Key_U:
	{
		_axisEye.Rotate(glm::radians(+angleDelta), glm::vec3{ 0.0f, 1.0f, 0.0f });
	}
	break;

	case Qt::Key_O:
	{
		_axisEye.Rotate(glm::radians(-angleDelta), glm::vec3{ 0.0f, 1.0f, 0.0f });
	}
	break;

	default:
		XQTBase::keyPressEvent(ev);
	}
	
}

XGLWindowTriangle::XGLWindowTriangle(QWindow*parent)
	: XQTBase(parent)
{
	//connect(ThreadGLRender(), &XThreadGLRender::RenderFrameDoneSignal, this, &XGLWindowTriangle::on_timeout);
	connect(this, &XGLWindowTriangle::RenderFrameDoneSignal, this, &XGLWindowTriangle::on_timeout);
}

XGLWindowTriangle::~XGLWindowTriangle()
{
	//FinalizeGL();
}
