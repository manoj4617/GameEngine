#ifndef STATE_H

#define STATE_H

struct ViewerSetup {

};

class State {
public:
	State(unsigned long id = 0);

	virtual void Load();
	virtual void Close();

	virtual void RequestViewer(ViewerSetup* viewer);
	virtual void Update(float elapsed);
	virtual void Render();

	unsigned long GetId();

private:
	unsigned long m_id;
};

#endif // !STATE_H

