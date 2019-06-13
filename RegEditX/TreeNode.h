#pragma once

enum class TreeNodeType {
	Generic,
	RegistryKey,
};

struct TreeNodeBase {
	friend class RegistryManager;

	TreeNodeBase(const CString& text, TreeNodeBase* parent = nullptr) : _parentNode(parent), _text(text) {}

	virtual ~TreeNodeBase() {}

	virtual TreeNodeType GetNodeType() const {
		return TreeNodeType::Generic;
	}

	void SetText(const CString& text) {
		_text = text;
	}

	HTREEITEM GetHItem() const {
		return _hItem;
	}

	const CString& GetText() const {
		return _text;
	}

	virtual int GetImage() const {
		return 0;
	}

	virtual int GetContextMenuIndex() const {
		return -1;
	}

	virtual int GetSelectedImage() const {
		return 1;
	}

	TreeNodeBase* GetParent() {
		return _parentNode;
	}

	TreeNodeBase* AddChild(TreeNodeBase* child);

	TreeNodeBase* RemoveChild(const CString& name);

	const std::vector<TreeNodeBase*>& GetChildNodes() const {
		return _childNodes;
	}

	TreeNodeBase* FindChild(const CString& text) const;

	virtual void Delete() {
		delete this;
	}

	virtual bool Expand(bool expand) {
		return false;
	}

	virtual bool IsExpanded() const {
		return false;
	}

	virtual bool CanDelete() const {
		return false;
	}

	virtual bool HasChildren() const {
		return false;
	}

	const CString& GetFullPath() const {
		return _full;
	}

private:
	void SetParent(TreeNodeBase* parent);

private:
	CString _text, _full;
	std::vector<TreeNodeBase*> _childNodes;
	TreeNodeBase* _parentNode;
	HTREEITEM _hItem{ nullptr };
};

