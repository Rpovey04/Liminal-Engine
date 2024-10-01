#include "ExternalDependencies.h"

template <typename T = float>
class Matrix {
protected:
	int X, Y;
	T** m;

	void offloadData() {
		for (int i = 0; i < X; i++) { delete[] m[i]; }
		if (X != 0) { delete[] m; }
	}

public:
	// CONSTRUCTS
	Matrix() : X(0), Y(0) {
		m = nullptr;
	}
	Matrix(int x, int y, bool fill = 0, T fillwith = 0) {
		X = x; Y = y;
		m = new T * [X];
		for (int i = 0; i < X; i++) { 
			m[i] = new T[Y]; 
			for (int j = 0; j < Y && fill; j++) { m[i][j] = fillwith; }
		}
	}
	~Matrix() {
		offloadData();
	}

	// GETTERS
	std::tuple<int, int> getDimensions() {
		return std::make_tuple(X, Y);
	}
	T** getData() {
		return m;
	}
	T* operator [](const int i) {		// returns an array which can then be manipulated with another [] as required
		return m[i];
	}

	// DIMENSION CHANGE
	void setDimension(int x, int y, bool fill = 0, T fillwith = 0) {
		if (X != 0 && Y != 0) { offloadData(); }
		X = x; Y = y;
		m = new T * [X];
		for (int i = 0; i < X; i++) {
			m[i] = new T[Y];
			for (int j = 0; j < Y && fill; j++) { m[i][j] = fillwith; }
		}
	}

	// DOT PRODUCT OVERLOADS
	Matrix<T>* operator * (Matrix<T>& mat) {	
		if (typeid(T) == typeid(float)) {
			if (Y == std::get<0>(mat.getDimensions())) {
				int Y2 = std::get<1>(mat.getDimensions());
				Matrix<float>* temp = new Matrix<float>(X, Y2, 1, 0);
				for (int i = 0; i < X; i++) {
					for (int j = 0; j < Y2; j++) {
						for (int k = 0; k < Y; k++) {
							(*temp)[i][j] += m[i][k] * mat[k][j];		// look at alloc method for this so I don't have to deal with pointers
						}
					}
				}
				return temp;
			}
		}
		return nullptr;
	}
	void operator *= (Matrix<T>& mat) {		// also program *= overload and change if to case
		if (typeid(T) == typeid(float)) {
			if (Y == std::get<0>(mat.getDimensions())) {
				int Y2 = std::get<1>(mat.getDimensions());
				T** newData = new T * [X];
				for (int w = 0; w < X; w++) {
					newData[w] = new T[Y2]; 
					for (int h = 0; h < Y2; h++) {
						newData[w][h] = 0;
					}
				}
				for (int i = 0; i < X; i++) {
					for (int j = 0; j < Y2; j++) {
						for (int k = 0; k < Y; k++) {
							newData[i][j] += m[i][k] * mat[k][j];
						}
					}
				}
				Y = Y2;
				for (int i = 0; i < X; i++) { delete[] m[i]; }
				if (X != 0) { delete[] m; }
				m = newData;
			}
		}
	}

	// ROTATION CONSTUCTION
	static Matrix<T>* RotMatrix(float rotX, float rotY, float rotZ) {	// all in degrees (amend to include rotation around non-origin point)
		Matrix<T>* mat = new Matrix<T>(4, 4, 1, 0);
		Matrix<float>::RotMatrix((*mat), rotX, rotY, rotZ);
		return mat;
	}
	static void RotMatrix(Matrix<T>& mat, float rotX, float rotY, float rotZ) {
		mat.setDimension(4, 4, 1, 0);

		float x = DegreeToRadian(rotX);
		float y = DegreeToRadian(rotY);
		float z = DegreeToRadian(rotZ);

		mat[0][0] = cosf(z) * cosf(y);
		mat[0][1] = -sinf(z) * cosf(x) + cosf(z) * sinf(y) * sinf(x);
		mat[0][2] = sinf(z) * sinf(x) + cosf(z) * sinf(y) * cosf(x);
		mat[1][0] = sinf(z) * cosf(y);
		mat[1][1] = cosf(z) * cosf(x) + sinf(z) * sinf(y) * sinf(x);
		mat[1][2] = -cosf(z) * sinf(x) + sinf(z) * sinf(y) * cosf(x);
		mat[2][0] = -sinf(y);
		mat[2][1] = cosf(y) * sinf(x);
		mat[2][2] = cosf(y) * cosf(x);
		mat[3][3] = 1.0f;
	}
	
	static void PointMatrix(Matrix<T>& mat, Matrix<T>& WorldPos, Matrix<T>& Target, Matrix<T>& TempUp) {
		mat.setDimension(4, 4, 1, 0 );
		if (typeid(T) == typeid(float)) {
			// Forward vector (straight line from Position to Target)
			Matrix<float> Forward;
			Matrix::SubVector(Forward, Target, WorldPos);
			Matrix::NormaliseVector(Forward);

			// Up vector (Line orthogonal to forward vector. May not be straight up if Forward involves elevation)
			Matrix<float> ChangeVector, Up;
			Matrix::MultiplyVector(ChangeVector, Forward, Matrix::VectorDotProduct(TempUp, Forward));
			Matrix::SubVector(Up, TempUp, ChangeVector);
			Matrix::NormaliseVector(Up);
			
			// Right vector (Perpendicular to both Forward and Right vectors, completing 3rd dimension)
			Matrix<float> Right;
			Matrix::VectorCrossProduct(Right, Up, Forward);
			Matrix::NormaliseVector(Right);

			// Point at matrix construction
			mat.setDimension(4, 4);
			mat[0][0] = Right[0][0];	mat[0][1] = Right[0][1];	mat[0][2] = Right[0][2];	mat[0][3] = 0.0f;
			mat[1][0] = Up[0][0];	mat[1][1] = Up[0][1];	mat[1][2] = Up[0][2];	mat[1][3] = 0.0f;
			mat[2][0] = Forward[0][0];	mat[2][1] = Forward[0][1];	mat[2][2] = Forward[0][2];	mat[2][3] = 0.0f;
			mat[3][0] = WorldPos[0][0];	mat[3][1] = WorldPos[0][1];	mat[3][2] = WorldPos[0][2];	mat[3][3] = 1.0f;
		}
	}
	static void LookMatrix(Matrix<T>& mat, Matrix<T>& WorldPos, Matrix<T>& Target, Matrix<T>& Up) {
		Matrix::PointMatrix(mat, WorldPos, Target, Up);
		Matrix::InverseTranformationMatrix(mat);
	}

	// FOR USE IN ENGINE
	static void HomogenousCoord(Matrix<T>& mat, float x, float y, float z, float w = 1) {
		mat.setDimension(1, 4, 1, 1);
		mat[0][0] = x;
		mat[0][1] = y;
		mat[0][2] = z;
		mat[0][3] = w;
	}


	// OTHER MATH FUNCTIONS
	static float LengthVector(Matrix<T>& vec) {
		if (typeid(T) == typeid(float)) {
			float l = 0;
			for (int i = 0; i < std::get<1>(vec.getDimensions()); i++) { l += (vec[0][i] * vec[0][i]); }
			return sqrt(l);
		}
		return 0;
	}
	static void NormaliseVector(Matrix<T>& vec) {
		float l = LengthVector(vec);
		if (typeid(T) == typeid(float)) {
			for (int i = 0; i < std::get<1>(vec.getDimensions()); i++) { vec[0][i] /= l; }
		}
	}
	static void VectorCrossProduct(Matrix<T>& vecWrite, Matrix<T>& vec1, Matrix<T>& vec2) {		// will only operate on first 3 elements (other dimensions are invalid or not useful)
		vecWrite.setDimension(std::get<0>(vec1.getDimensions()), std::get<1>(vec1.getDimensions()), 1, 0);
		if (typeid(T) == typeid(float)) {
			vecWrite[0][0] = (vec1[0][1] * vec2[0][2]) - (vec1[0][2] * vec2[0][1]);
			vecWrite[0][1] = (vec1[0][2] * vec2[0][0]) - (vec1[0][0] * vec2[0][2]);
			vecWrite[0][2] = (vec1[0][0] * vec2[0][1]) - (vec1[0][1] * vec2[0][0]);
			if (std::get<1>(vec1.getDimensions()) == 3) { vecWrite[0][3] = vec1[0][3]; }
		}
	}
	static float VectorDotProduct(Matrix<T>& vec1, Matrix<T>& vec2) {
		if (typeid(T) == typeid(float)) {
			float total = 0;
			for (int i = 0; i < std::get<1>(vec1.getDimensions()); i++) { total += vec1[0][i] * vec2[0][i]; }
			return total;
		}
		return 0;
	}

	static void AddVector(Matrix<T>& vecWrite, Matrix<T>& vec1, Matrix<T>& vec2) {
		vecWrite.setDimension(std::get<0>(vec1.getDimensions()), std::get<1>(vec1.getDimensions()), 1, 0);
		if (typeid(T) == typeid(float)) {
			for (int i = 0; i < std::get<1>(vec1.getDimensions()); i++) { vecWrite[0][i] = vec1[0][i] + vec2[0][i]; }
		}
	}
	static void SubVector(Matrix<T>& vecWrite, Matrix<T>& vec1, Matrix<T>& vec2) {
		vecWrite.setDimension(std::get<0>(vec1.getDimensions()), std::get<1>(vec1.getDimensions()), 1, 0);
		if (typeid(T) == typeid(float)) {
			for (int i = 0; i < std::get<1>(vec1.getDimensions()); i++) { vecWrite[0][i] = vec1[0][i] - vec2[0][i]; }
		}
	}
	static void MultiplyVector(Matrix<T>& vecWrite, Matrix<T>& vec1, float mul) {
		vecWrite.setDimension(std::get<0>(vec1.getDimensions()), std::get<1>(vec1.getDimensions()), 1, 0);
		if (typeid(T) == typeid(float)) {
			for (int i = 0; i < std::get<1>(vec1.getDimensions()); i++) { vecWrite[0][i] = vec1[0][i] * mul; }
		}
	}
	static void DivideVector(Matrix<T>& vecWrite, Matrix<T>& vec1, float div) {
		vecWrite.setDimension(std::get<0>(vec1.getDimensions()), std::get<1>(vec1.getDimensions()), 1, 0);
		if (typeid(T) == typeid(float)) {
			for (int i = 0; i < std::get<1>(vec1.getDimensions()); i++) { vecWrite[0][i] = vec1[0][i] / div; }
		}
	}

	static void InverseTranformationMatrix(Matrix<T>& mat) {
		T** newData = new T*[4];
		for (int i = 0; i < 4; i++) { newData[i] = new T[4]; }

		newData[0][0] = mat[0][0]; newData[0][1] = mat[1][0]; newData[0][2] = mat[2][0]; newData[0][3] = 0.0f;
		newData[1][0] = mat[0][1]; newData[1][1] = mat[1][1]; newData[1][2] = mat[2][1]; newData[1][3] = 0.0f;
		newData[2][0] = mat[0][2]; newData[2][1] = mat[1][2]; newData[2][2] = mat[2][2]; newData[2][3] = 0.0f;
		newData[3][0] = -(mat[3][0] * newData[0][0] + mat[3][1] * newData[1][0] + mat[3][2] * newData[2][0]);
		newData[3][1] = -(mat[3][0] * newData[0][1] + mat[3][1] * newData[1][1] + mat[3][2] * newData[2][1]);
		newData[3][2] = -(mat[3][0] * newData[0][2] + mat[3][1] * newData[1][2] + mat[3][2] * newData[2][2]);
		newData[3][3] = 1.0f;

		mat.offloadData();
		mat.m = newData;
	}

	// FOR PRINT
	#ifdef _DEBUG
	friend std::ostream& operator<<(std::ostream& os, Matrix& mat);
	#endif
};

// DISPLAY OVERLOADS
#ifdef _DEBUG
static std::ostream& operator<<(std::ostream& os, Matrix<float>& mat) {
	for (int x = 0; x < std::get<0>(mat.getDimensions()); x++) { 
		for (int y = 0; y < std::get<1>(mat.getDimensions()); y++) { os << mat[x][y] << "\t"; }
		os << std::endl;
	}
	return os;
}
static std::ostream& operator<<(std::ostream& os, Matrix<double>& mat) {
	for (int x = 0; x < std::get<0>(mat.getDimensions()); x++) {
		for (int y = 0; y < std::get<1>(mat.getDimensions()); y++) { os << mat[x][y] << "\t"; }
		os << std::endl;
	}
	return os;
}
static std::ostream& operator<<(std::ostream& os, Matrix<int>& mat) {
	for (int x = 0; x < std::get<0>(mat.getDimensions()); x++) {
		for (int y = 0; y < std::get<1>(mat.getDimensions()); y++) { os << mat[x][y] << "\t"; }
		os << std::endl;
	}
	return os;
}
static std::ostream& operator<<(std::ostream& os, Matrix<char>& mat) {
	for (int x = 0; x < std::get<0>(mat.getDimensions()); x++) {
		for (int y = 0; y < std::get<1>(mat.getDimensions()); y++) { os << mat[x][y] << "\t"; }
		os << std::endl;
	}
	return os;
}
#endif