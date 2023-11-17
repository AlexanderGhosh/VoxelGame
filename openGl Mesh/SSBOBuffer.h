#pragma once
class Buffer {
protected:
	Buffer() {};
public:
	virtual void bind() const = 0;
	virtual void unBind() const = 0;
	virtual void cleanUp() = 0;
	virtual void setUp(void* data, unsigned int size) = 0;
};
class SSBOBuffer : public Buffer
{
public:
	SSBOBuffer();
	~SSBOBuffer();

	void bind() const override;
	void unBind() const override;
	void cleanUp() override;
	void setUp(void* data, unsigned int size) override;

	void setBindingPoint(const unsigned int point);
private:
	unsigned int ssbo_;
	unsigned int bindingPoint_;
};

